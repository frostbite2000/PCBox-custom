/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          ATI Rage II+ DVD (based on Mach64 architecture) graphics card emulation.
 *
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000.
 */
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <stdatomic.h>
#define HAVE_STDARG_H
#include <86box/86box.h>
#include <86box/device.h>
#include <86box/io.h>
#include <86box/mem.h>
#include "cpu.h"
#include <86box/timer.h>
#include <86box/pci.h>
#include <86box/rom.h>
#include <86box/plat.h>
#include <86box/thread.h>
#include <86box/video.h>
#include <86box/i2c.h>
#include <86box/vid_ddc.h>
#include <86box/vid_svga.h>
#include <86box/vid_svga_render.h>
#include <86box/vid_ati_eeprom.h>
#include <86box/ati/vid_rageiiidvd.h>

// Path to the ROM
#define BIOS_RAGEIIDVD_ROM_PATH "roms/video/ati/rageiidvd/2mbsgr.VBI"

// MACH64 post-dividers for PLL
static const int pll_post_dividers[8] = {
    1, 2, 4, 8, 3, 5, 6, 12
};

#ifdef ENABLE_RAGEIIDVD_LOG
int rageiidvd_do_log = ENABLE_RAGEIIDVD_LOG;

static void
rageiidvd_log(const char *fmt, ...)
{
    va_list ap;

    if (rageiidvd_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#define rageiidvd_log(fmt, ...)
#endif

typedef struct rageiidvd_t {
    svga_t      svga;
    ati_eeprom_t eeprom;

    rom_t       bios_rom;
    
    uint8_t     regs[256];
    uint8_t     pci_regs[256];
    uint8_t     pll_regs[16];

    uint32_t    memory;
    uint8_t     pci_slot;
    int         ramdac_type;
    
    uint8_t     dac_mask;
    uint8_t     dac_state;
    uint8_t     dac_windex;
    uint8_t     dac_rindex;
    uint8_t     dac_colors[256 * 3];

    int         hres, vres;
    int         htotal, vtotal;
    int         format;
    int         pixel_clock;

    mem_mapping_t mmio_mapping;
    mem_mapping_t linear_mapping;
    
    void        *i2c, *ddc;
} rageiidvd_t;

// Forward declarations
static uint8_t rageiidvd_mmio_read(uint32_t addr, void *priv);
static void rageiidvd_mmio_write(uint32_t addr, uint8_t val, void *priv);
static void rageiidvd_update_mode(rageiidvd_t *rageiidvd);

/* PCI handler functions */
static uint8_t rageiidvd_pci_read(int func, int addr, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    uint8_t ret = 0;

    if (func)
        return 0xff;

    switch (addr) {
        case 0x00:
            ret = 0x02; /* ATI */
            break;
        case 0x01:
            ret = 0x10;
            break;
        case 0x02:
            ret = (ATI_RAGEIIDVD_PCI_DEVICE_ID & 0xff);
            break;
        case 0x03:
            ret = (ATI_RAGEIIDVD_PCI_DEVICE_ID >> 8);
            break;
        case 0x04:
            ret = (rageiidvd->pci_regs[0x04] & 0x17) | 0x80; /* Respond to IO and memory accesses */
            break;
        case 0x05:
            ret = rageiidvd->pci_regs[0x05] & 0x03;
            break;
        case 0x06:
            ret = 0x80; /* Respond to PALETTE accesses */
            break;
        case 0x07:
            ret = 0x02; /* Fast DEVSEL timing */
            break;
        case 0x08:
            ret = 0x3a; /* Revision ID */
            break;
        case 0x09:
            ret = 0x00; /* Programming interface */
            break;
        case 0x0a:
            ret = 0x00; /* Supports VGA interface */
            break;
        case 0x0b:
            ret = 0x03; /* Display class */
            break;
        case 0x10:
            ret = 0x00; /* Linear framebuffer address */
            break;
        case 0x11:
            ret = 0x00;
            break;
        case 0x12:
            ret = 0x00;
            break;
        case 0x13:
            ret = rageiidvd->pci_regs[0x13];
            break;
        case 0x14:
            ret = 0x01; /* IO Base */
            break;
        case 0x15:
            ret = rageiidvd->pci_regs[0x15]; /* IO Base high byte */
            break;
        case 0x16:
            ret = rageiidvd->pci_regs[0x16];
            break;
        case 0x17:
            ret = rageiidvd->pci_regs[0x17];
            break;
        case 0x18:
            ret = 0x00; /* MMIO Base */
            break;
        case 0x19:
            ret = 0x00;
            break;
        case 0x1a:
            ret = 0x00;
            break;
        case 0x1b:
            ret = rageiidvd->pci_regs[0x1b]; /* MMIO Base high byte */
            break;
        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            ret = rageiidvd->pci_regs[addr];
            break;
        case 0x30:
            ret = (rageiidvd->pci_regs[0x30] & 0x01); /* BIOS ROM address */
            break;
        case 0x31:
            ret = 0x00;
            break;
        case 0x32:
            ret = rageiidvd->pci_regs[0x32];
            break;
        case 0x33:
            ret = rageiidvd->pci_regs[0x33];
            break;
        case 0x3c:
            ret = rageiidvd->pci_regs[0x3c];
            break;
        case 0x3d:
            ret = PCI_INTA;
            break;
        default:
            ret = 0x00;
            break;
    }

    rageiidvd_log("rageiidvd_pci_read: addr=%02x ret=%02x\n", addr, ret);

    return ret;
}

static void
rageiidvd_pci_write(int func, int addr, uint8_t val, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    uint8_t old;

    if (func)
        return;

    rageiidvd_log("rageiidvd_pci_write: addr=%02x val=%02x\n", addr, val);

    switch (addr) {
        case 0x04:
            old = rageiidvd->pci_regs[addr];
            rageiidvd->pci_regs[addr] = val & 0x17;
            if ((val & PCI_COMMAND_IO) && !(old & PCI_COMMAND_IO)) {
                rageiidvd_log("PCI IO enabled\n");
            } else if (!(val & PCI_COMMAND_IO) && (old & PCI_COMMAND_IO)) {
                rageiidvd_log("PCI IO disabled\n");
            }
            break;

        case 0x05:
            rageiidvd->pci_regs[addr] = val & 0x03;
            break;

        case 0x13:
            rageiidvd->pci_regs[addr] = val;
            rageiidvd_log("PCI Memory base=%08x\n", rageiidvd->pci_regs[addr] << 24);
            break;

        case 0x15:
        case 0x16:
        case 0x17:
            rageiidvd->pci_regs[addr] = val;
            break;

        case 0x1b:
            rageiidvd->pci_regs[addr] = val;
            rageiidvd_log("PCI MMIO base=%08x\n", rageiidvd->pci_regs[addr] << 24);
            break;

        case 0x2c:
        case 0x2d:
        case 0x2e:
        case 0x2f:
            rageiidvd->pci_regs[addr] = val;
            break;

        case 0x30:
        case 0x32:
        case 0x33:
            rageiidvd->pci_regs[addr] = val;
            if (rageiidvd->pci_regs[0x30] & 0x01) {
                uint32_t biosaddr = (rageiidvd->pci_regs[0x32] << 16) | (rageiidvd->pci_regs[0x33] << 24);
                rageiidvd_log("BIOS ROM PCI mapping enabled at %08x\n", biosaddr);
                mem_mapping_set_addr(&rageiidvd->bios_rom.mapping, biosaddr, 0x8000);
            } else {
                rageiidvd_log("BIOS ROM PCI mapping disabled\n");
                mem_mapping_disable(&rageiidvd->bios_rom.mapping);
            }
            break;

        case 0x3c:
            rageiidvd->pci_regs[addr] = val;
            break;
    }
}

/* Memory mapper functions */
static uint8_t
rageiidvd_mmio_read(uint32_t addr, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    addr &= 0xfff;
    rageiidvd_log("MMIO read: %08x\n", addr);
    
    if (addr < 0x400) {
        return rageiidvd->regs[addr];
    }
    
    return 0xff;
}

static uint16_t
rageiidvd_mmio_read_w(uint32_t addr, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    addr &= 0xfff;
    rageiidvd_log("MMIO read w: %08x\n", addr);
    
    if (addr < 0x3ff) {
        return rageiidvd->regs[addr] | (rageiidvd->regs[addr + 1] << 8);
    }
    
    return 0xffff;
}

static uint32_t
rageiidvd_mmio_read_l(uint32_t addr, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    addr &= 0xfff;
    rageiidvd_log("MMIO read l: %08x\n", addr);
    
    if (addr < 0x3fd) {
        return rageiidvd->regs[addr] | (rageiidvd->regs[addr + 1] << 8) | 
               (rageiidvd->regs[addr + 2] << 16) | (rageiidvd->regs[addr + 3] << 24);
    }
    
    return 0xffffffff;
}

static void
rageiidvd_mmio_write(uint32_t addr, uint8_t val, void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    uint8_t old;
    
    addr &= 0xfff;
    old = rageiidvd->regs[addr];
    rageiidvd->regs[addr] = val;
    
    rageiidvd_log("MMIO write: %08x = %02x (old=%02x)\n", addr, val, old);
    
    switch (addr) {
        case CRTC_DAC_BASE:
            rageiidvd->dac_state = 0;
            rageiidvd->dac_windex = val;
            break;
            
        case CRTC_DAC_BASE + 1:
            if (rageiidvd->dac_state == 0) {
                rageiidvd->dac_colors[rageiidvd->dac_windex * 3] = val;
                rageiidvd->dac_state = 1;
            } else if (rageiidvd->dac_state == 1) {
                rageiidvd->dac_colors[rageiidvd->dac_windex * 3 + 1] = val;
                rageiidvd->dac_state = 2;
            } else {
                rageiidvd->dac_colors[rageiidvd->dac_windex * 3 + 2] = val;
                rageiidvd->dac_state = 0;
                rageiidvd->svga.pallook[rageiidvd->dac_windex] = makecol32(
                    rageiidvd->dac_colors[rageiidvd->dac_windex * 3],
                    rageiidvd->dac_colors[rageiidvd->dac_windex * 3 + 1],
                    rageiidvd->dac_colors[rageiidvd->dac_windex * 3 + 2]);
                rageiidvd->dac_windex++;
            }
            break;
            
        case CRTC_DAC_BASE + 2:
            rageiidvd->dac_mask = val;
            break;
            
        case CRTC_DAC_BASE + 3:
            rageiidvd->dac_state = 0;
            rageiidvd->dac_rindex = val;
            break;
            
        case CRTC_OFF_PITCH:
        case CRTC_GEN_CNTL:
            // Update mode when CRTC registers change
            rageiidvd_update_mode(rageiidvd);
            break;
            
        case CLOCK_CNTL + 2:
            if (rageiidvd->regs[CLOCK_CNTL + 1] & 0x02) {
                uint8_t regnum = (rageiidvd->regs[CLOCK_CNTL + 1] >> 2) & 0x0f;
                rageiidvd->pll_regs[regnum] = val;
                rageiidvd_log("PLL reg %d = %02x\n", regnum, val);
            }
            break;
    }
}

static void
rageiidvd_mmio_write_w(uint32_t addr, uint16_t val, void *priv)
{
    rageiidvd_mmio_write(addr, val & 0xff, priv);
    rageiidvd_mmio_write(addr + 1, val >> 8, priv);
}

static void
rageiidvd_mmio_write_l(uint32_t addr, uint32_t val, void *priv)
{
    rageiidvd_mmio_write(addr, val & 0xff, priv);
    rageiidvd_mmio_write(addr + 1, (val >> 8) & 0xff, priv);
    rageiidvd_mmio_write(addr + 2, (val >> 16) & 0xff, priv);
    rageiidvd_mmio_write(addr + 3, (val >> 24) & 0xff, priv);
}

static void
rageiidvd_update_mode(rageiidvd_t *rageiidvd)
{
    // First prereq: must be in native mode and the CRTC must be enabled
    if (!(rageiidvd->regs[CRTC_GEN_CNTL + 3] & 3)) {
        rageiidvd_log("VGA mode must be OFF and CRTC must be ON\n");
        return;
    }

    rageiidvd->htotal = (rageiidvd->regs[CRTC_H_TOTAL_DISP] | (rageiidvd->regs[CRTC_H_TOTAL_DISP + 1] & 1) << 8) + 1;
    rageiidvd->htotal <<= 3; // in units of 8 pixels
    rageiidvd->hres = rageiidvd->regs[CRTC_H_TOTAL_DISP + 2] + 1;
    rageiidvd->hres <<= 3;
    rageiidvd->vres = (rageiidvd->regs[CRTC_V_TOTAL_DISP + 2] | (rageiidvd->regs[CRTC_V_TOTAL_DISP + 3] & 7) << 8) + 1;
    rageiidvd->vtotal = (rageiidvd->regs[CRTC_V_TOTAL_DISP] | (rageiidvd->regs[CRTC_V_TOTAL_DISP + 1] & 7) << 8) + 1;
    rageiidvd->format = rageiidvd->regs[CRTC_GEN_CNTL + 1] & 7;
    
    rageiidvd_log("Setting mode (%d x %d), total (%d x %d) format %d\n", 
                 rageiidvd->hres, rageiidvd->vres, rageiidvd->htotal, rageiidvd->vtotal, rageiidvd->format);

    // Calculate pixel clock from PLL settings
    double vpll_frequency;
    int clk_source = rageiidvd->regs[CLOCK_CNTL] & 3;
    int vpll_ref_div = rageiidvd->pll_regs[PLL_REF_DIV] & 0x3f;
    
    if (vpll_ref_div == 0)
        vpll_ref_div = 1;
        
    double ref_freq = 14318180.0f; // Standard VGA reference frequency
    
    int vpll_fb_div = 0;
    
    switch (clk_source) {
        case 0:
            vpll_fb_div = rageiidvd->pll_regs[VCLK0_FB_DIV] & 0xff;
            break;
        case 1:
            vpll_fb_div = rageiidvd->pll_regs[VCLK1_FB_DIV] & 0xff;
            break;
        case 2:
            vpll_fb_div = rageiidvd->pll_regs[VCLK2_FB_DIV] & 0xff;
            break;
        case 3:
            vpll_fb_div = rageiidvd->pll_regs[VCLK3_FB_DIV] & 0xff;
            break;
    }
    
    if (vpll_fb_div == 0)
        vpll_fb_div = 1;
        
    vpll_frequency = ref_freq * (double)vpll_fb_div / (double)vpll_ref_div;
    
    int vpll_post_divider = rageiidvd->pll_regs[VCLK_POST_DIV] & 3;
    
    // Rage Pro adds one more bit to the divider from bits 4/5/6/7 of XCLK_CNTL depending on the clock source.
    // This should always be zero on mach64/Rage/Rage II.
    vpll_post_divider |= ((rageiidvd->pll_regs[PLL_XCLK_CNTL] >> (clk_source + 2)) & 4);
    
    rageiidvd->pixel_clock = (int)(vpll_frequency / pll_post_dividers[vpll_post_divider]);
    
    rageiidvd_log("Pixel clock = %d, refresh = %f\n", 
                 rageiidvd->pixel_clock, 
                 (double)rageiidvd->pixel_clock / (double)rageiidvd->htotal / (double)rageiidvd->vtotal);
                 
    // Update SVGA timing based on the calculated values
    if (rageiidvd->svga.recalctimings_ex) {
        rageiidvd->svga.recalctimings_ex(&rageiidvd->svga);
    }
}

static void
rageiidvd_recalctimings(svga_t *svga)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)svga->priv;
    
    rageiidvd_log("rageiidvd_recalctimings called\n");
    
    // Set up the screen timing using the calculated values
    if (rageiidvd->hres > 0 && rageiidvd->vres > 0 && rageiidvd->htotal > 0 && rageiidvd->vtotal > 0) {
        svga->crtc[0x11] = 0x00; // Unlock CRTC registers
        
        svga->htotal = rageiidvd->htotal;
        svga->dispend = rageiidvd->hres;
        svga->vtotal = rageiidvd->vtotal;
        svga->vdispend = rageiidvd->vres;
    }
}

static void *
rageiidvd_init(const device_t *info)
{
    rageiidvd_t *rageiidvd = calloc(1, sizeof(rageiidvd_t));
    
    rageiidvd_log("Initializing ATI Rage II+ DVD\n");
    
    video_inform(VIDEO_FLAG_TYPE_SPECIAL, &timing_mach64_pci);
    
    // Set up ROM
    rom_init(&rageiidvd->bios_rom, BIOS_RAGEIIDVD_ROM_PATH, 0xc0000, 0x8000, 0x7fff, 0, MEM_MAPPING_EXTERNAL);
    
    // Set default memory size (4MB or 8MB typical for this card)
    rageiidvd->memory = device_get_config_int("memory");
    
    // Initialize SVGA
    svga_init(info, &rageiidvd->svga, rageiidvd, rageiidvd->memory << 10,
              rageiidvd_recalctimings,
              NULL, NULL, NULL, NULL);
              
    // Set RAMDAC for Mach64
    rageiidvd->svga.ramdac = device_add(&ati68860_ramdac_device);
    
    // Set up I2C and DDC
    rageiidvd->i2c = i2c_gpio_init("ati_rageiidvd_i2c");
    rageiidvd->ddc = ddc_init(i2c_gpio_get_bus(rageiidvd->i2c));
    
    // Set up memory mappings
    mem_mapping_add(&rageiidvd->mmio_mapping, 0, 0, 
                   rageiidvd_mmio_read, rageiidvd_mmio_read_w, rageiidvd_mmio_read_l,
                   rageiidvd_mmio_write, rageiidvd_mmio_write_w, rageiidvd_mmio_write_l,
                   NULL, MEM_MAPPING_EXTERNAL, rageiidvd);
    mem_mapping_disable(&rageiidvd->mmio_mapping);
    
    // Set up PCI
    rageiidvd->pci_regs[0x30] = 0x00;
    rageiidvd->pci_regs[0x32] = 0x0c;
    rageiidvd->pci_regs[0x33] = 0x00;
    
    pci_add_card(PCI_ADD_VIDEO, rageiidvd_pci_read, rageiidvd_pci_write, rageiidvd, &rageiidvd->pci_slot);
    
    // Initialize CHIP_ID registers
    rageiidvd->regs[CONFIG_CHIP_ID] = 0x55;      // ASCII 'U'
    rageiidvd->regs[CONFIG_CHIP_ID + 1] = 0x47;  // ASCII 'G'
    rageiidvd->regs[CONFIG_CHIP_ID + 3] = 0x3a;  // Revision
    
    // Load EEPROM configuration
    ati_eeprom_load(&rageiidvd->eeprom, "ati_rageiidvd.nvr", 0);
    
    // Initialize the display format to a safe default
    rageiidvd->format = 0;  // 8-bit paletted mode
    
    return rageiidvd;
}

static void
rageiidvd_close(void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    rageiidvd_log("Closing ATI Rage II+ DVD\n");
    
    svga_close(&rageiidvd->svga);
    
    free(rageiidvd);
}

static void
rageiidvd_speed_changed(void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    svga_recalctimings(&rageiidvd->svga);
}

static void
rageiidvd_force_redraw(void *priv)
{
    rageiidvd_t *rageiidvd = (rageiidvd_t *)priv;
    
    rageiidvd->svga.fullchange = changeframecount;
}

static int
rageiidvd_available(void)
{
    return rom_present(BIOS_RAGEIIDVD_ROM_PATH);
}

static const device_config_t rageiidvd_config[] = {
    {
        .name = "memory",
        .description = "Memory size",
        .type = CONFIG_SELECTION,
        .default_int = 200000,
        .selection = {
            {
                .description = "2 MB",
                .value = 200000
            },
            {
                .description = "4 MB",
                .value = 400000
            },
            {
                .description = "8 MB",
                .value = 800000
            },
            {
                .description = ""
            }
        }
    },
    {
        .type = CONFIG_END
    }
};

const device_t ati_rageiidvd_pci_device = {
    .name = "ATI Rage II+ DVD",
    .internal_name = "ati_rageiidvd_pci",
    .flags = DEVICE_PCI,
    .local = 0,
    .init = rageiidvd_init,
    .close = rageiidvd_close,
    .reset = NULL,
    .available = rageiidvd_available,
    .speed_changed = rageiidvd_speed_changed,
    .force_redraw = rageiidvd_force_redraw,
    .config = rageiidvd_config
};