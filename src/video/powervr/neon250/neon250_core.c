/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          VideoLogic PowerVR Neon 250 (NEC PMX1) emulation core.
 *
 * Authors: frostbite2000,
 *
 *          Copyright 2025 frostbite2000.
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdbool.h>
#define HAVE_STDARG_H

#include "86box/86box.h"
#include "86box/io.h"
#include "86box/mem.h"
#include "86box/pci.h"
#include "86box/device.h"
#include "86box/timer.h"
#include "86box/plat.h"
#include "86box/video.h"
#include "86box/vid_svga.h"

#include "86box/vid_powervr_neon250.h"

/* For the svga_mark_dirty function */
#define svga_mark_dirty(addr, y) do { } while (0)

/* Neon 250 Device IDs */
#define NEON250_VENDOR_ID     0x1033  /* NEC vendor ID */
#define NEON250_DEVICE_ID     0x0067  /* Neon 250 device ID */

/* Neon 250 register definitions */
#define NEON_REG_INTSTATUS    0x00    /* Interrupt Status Register */
#define NEON_REG_INTMASK      0x01    /* Interrupt Mask Register */
#define NEON_REG_SOFTRESET    0x02    /* Soft Reset Register */
#define NEON_REG_POWERDOWN    0x03    /* Power Down Register */
#define NEON_REG_TEXCTRL      0x04    /* Texture Control Register */
#define NEON_REG_FOGCTRL      0x05    /* Fog Control Register */
#define NEON_REG_FOGCOLOR     0x06    /* Fog Color Register */
#define NEON_REG_RENDERCTRL   0x07    /* Render Control Register */
#define NEON_REG_OBJECTBASE   0x08    /* Object Base Address Register */
#define NEON_REG_FRAMEBASE    0x09    /* Frame Buffer Base Address Register */
#define NEON_REG_TEXBASE      0x0A    /* Texture Memory Base Address Register */

/* Neon 250 flags */
#define NEON250_FLAG_FOG_ENABLE      0x01    /* Fog enabled */
#define NEON250_FLAG_BILINEAR_ENABLE 0x02    /* Bilinear filtering enabled */
#define NEON250_FLAG_DITHER_ENABLE   0x04    /* Dithering enabled */

/* Neon 250 rendering states */
#define NEON250_RENDER_STATE_IDLE     0      /* No rendering in progress */
#define NEON250_RENDER_STATE_ACTIVE   1      /* Rendering in progress */
#define NEON250_RENDER_STATE_COMPLETE 2      /* Rendering completed */

#ifdef ENABLE_NEON250_LOG
int neon250_do_log = ENABLE_NEON250_LOG;

static void
neon250_log(const char *fmt, ...)
{
    va_list ap;
    
    if (neon250_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#define neon250_log(fmt, ...)
#endif

static video_timings_t timing_neon250 = {
    .type = VIDEO_AGP,
    .write_b = 4,
    .write_w = 4,
    .write_l = 8,
    .read_b = 20,
    .read_w = 20,
    .read_l = 24
};

/* Function prototypes */
static void     neon250_update_irq(neon250_t *neon250);
static uint32_t neon250_reg_read(uint32_t addr, void *priv);
static void     neon250_reg_write(uint32_t addr, uint32_t val, void *priv);
static uint8_t  neon250_pci_read(int func, int addr, void *priv);
static void     neon250_pci_write(int func, int addr, uint8_t val, void *priv);
static void     neon250_recalc_mapping(neon250_t *neon250);
static void     neon250_init_registers(neon250_t *neon250);
static void     neon250_render_frame(neon250_t *neon250);

/* VGA pass-through functions */
static uint8_t  neon250_vram_read(uint32_t addr, void *priv);
static void     neon250_vram_write(uint32_t addr, uint8_t val, void *priv);

/* SVGA callback functions for Neon 250 */
static void neon250_svga_recalctimings(svga_t *svga);
static void neon250_svga_out(uint16_t addr, uint8_t val, void *priv);
static uint8_t neon250_svga_in(uint16_t addr, void *priv);
static void neon250_svga_write_linear(uint32_t addr, uint8_t val, void *priv);
static uint8_t neon250_svga_read_linear(uint32_t addr, void *priv);

/* VGA CRTC registers for Neon 250 */
#define NEON250_CRTC_H_TOTAL      0x00
#define NEON250_CRTC_H_DISPLAY    0x01
#define NEON250_CRTC_H_SYNC_START 0x04
#define NEON250_CRTC_H_SYNC_END   0x05
#define NEON250_CRTC_V_TOTAL      0x06
#define NEON250_CRTC_V_DISPLAY    0x12
#define NEON250_CRTC_V_SYNC_START 0x10
#define NEON250_CRTC_V_SYNC_END   0x11
#define NEON250_CRTC_MODE_CONTROL 0x17

/* Note: Extended registers are now defined in the header file */

/* MMIO register read function */
static uint32_t
neon250_reg_read(uint32_t addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    uint32_t offset = (addr & 0x3FF) >> 2;  /* Convert to DWORD offset */
    uint32_t ret = 0xFFFFFFFF;
    
    if (offset >= 0x100 && offset < 0x200) {
        /* 3D registers (0x100-0x1FF) */
        ret = neon_3d_read(neon250, offset << 2);
    } else if (offset < 256) {
        /* Core registers (0x00-0xFF) */
        ret = neon250->regs[offset];
    }
    
    neon250_log("NEON250: Register read offset=%04X val=%08X\n", offset, ret);
    
    return ret;
}

/* MMIO register write function */
static void
neon250_reg_write(uint32_t addr, uint32_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    uint32_t offset = (addr & 0x3FF) >> 2;  /* Convert to DWORD offset */
    
    neon250_log("NEON250: Register write offset=%04X val=%08X\n", offset, val);
    
    if (offset >= 0x100 && offset < 0x200) {
        /* 3D registers (0x100-0x1FF) */
        neon_3d_write(neon250, offset << 2, val);
        return;
    }
    
    if (offset >= 256)
        return;
        
    switch (offset) {
        case NEON_REG_INTSTATUS:
            /* Writing to INTSTATUS clears the bits */
            neon250->regs[offset] &= ~val;
            neon250_update_irq(neon250);
            break;
            
        case NEON_REG_SOFTRESET:
            /* Soft reset - re-initialize registers */
            if (val & 0x01) {
                neon250_log("NEON250: Soft reset\n");
                neon250_init_registers(neon250);
                neon250->regs[NEON_REG_INTSTATUS] = 0;
                neon250_update_irq(neon250);
                
                /* Also reset the 3D engine */
                if (neon250->state_3d) {
                    neon_3d_reset(neon250);
                }
            }
            break;
            
        case NEON_REG_RENDERCTRL:
            /* Start rendering if bit 0 is set */
            if (val & 0x01) {
                neon250_log("NEON250: Start rendering\n");
                neon250->render_state = NEON250_RENDER_STATE_ACTIVE;
                neon250_render_frame(neon250);
            }
            neon250->regs[offset] = val;
            break;
            
        case NEON_REG_FOGCTRL:
            /* Update fog enable flag */
            if (val & 0x01) {
                neon250->flags |= NEON250_FLAG_FOG_ENABLE;
            } else {
                neon250->flags &= ~NEON250_FLAG_FOG_ENABLE;
            }
            neon250->regs[offset] = val;
            break;
            
        case NEON_REG_TEXCTRL:
            /* Update bilinear filtering and dithering flags */
            if (val & 0x01) {
                neon250->flags |= NEON250_FLAG_BILINEAR_ENABLE;
            } else {
                neon250->flags &= ~NEON250_FLAG_BILINEAR_ENABLE;
            }
            
            if (val & 0x02) {
                neon250->flags |= NEON250_FLAG_DITHER_ENABLE;
            } else {
                neon250->flags &= ~NEON250_FLAG_DITHER_ENABLE;
            }
            neon250->regs[offset] = val;
            break;
            
        case NEON_REG_FOGCOLOR:
            /* Set fog color */
            neon250->fog_color = val & 0x00FFFFFF;  /* 24-bit RGB color */
            neon250->regs[offset] = val;
            break;
            
        case NEON_REG_POWERDOWN:
            /* Handle power down */
            neon250_log("NEON250: Power %s\n", (val & 0x01) ? "down" : "up");
            neon250->regs[offset] = val;
            break;
            
        default:
            /* Store the value in the register array */
            neon250->regs[offset] = val;
            break;
    }
}

/* Update IRQ function */
static void
neon250_update_irq(neon250_t *neon250)
{
    uint32_t status = neon250->regs[NEON_REG_INTSTATUS];
    uint32_t mask = neon250->regs[NEON_REG_INTMASK];
    
    /* Check if any enabled interrupts are active */
    if (status & mask) {
        neon250->irq_state = 1;
    } else {
        neon250->irq_state = 0;
    }
    
    /* Update PCI interrupt line */
    pci_set_irq(neon250->pci_slot, PCI_INTA, &neon250->irq_state);
}

/* PCI configuration read function */
static uint8_t
neon250_pci_read(int func, int addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    uint8_t ret = 0xFF;
    
    if (func > 0)
        return ret;
    
    ret = neon250->pci_regs[addr / 4] >> ((addr % 4) * 8);
    
    switch (addr) {
        case 0x00: return (NEON250_VENDOR_ID & 0xFF);
        case 0x01: return (NEON250_VENDOR_ID >> 8);
    
        case 0x02: return (NEON250_DEVICE_ID & 0xFF);
        case 0x03: return (NEON250_DEVICE_ID >> 8);
        
        case 0x04: return neon250->pci_regs[0x04] & 0x37;
        case 0x05: return 0x00;
    
        case 0x06: return 0x90;
        case 0x07: return 0x02;
        
        case 0x08: return 0x02; /* Revision ID */
        case 0x09: return 0x00; /* Programming interface */
    
        case 0x0a: return 0x00; /* Subclass - Display/3D */
        case 0x0b: return 0x03; /* Class - VGA controller */
        
        case 0x10: return 0x08; /* Framebuffer base address */
        case 0x11: return 0x00;
        case 0x12: return 0x00;
        case 0x13: return neon250->fb_base >> 24;
    
        case 0x14: return 0x00; /* MMIO base address */
        case 0x15: return 0x00;
        case 0x16: return 0x00;
        case 0x17: return neon250->mmio_base >> 24;

        case 0x2c: case 0x2d: case 0x2e: case 0x2f:
            return neon250->pci_regs[addr];

        case 0x30: return (neon250->pci_regs[0x30] & 0x01); /*BIOS ROM address*/
        case 0x31: return 0x00;
        case 0x32: return neon250->pci_regs[0x32];
        case 0x33: return neon250->pci_regs[0x33];

        case 0x34: return 0x60;

        case 0x60: return 0x01;
        case 0x61: return 0x40;
        case 0x62: return 0x21;
        case 0x63: return 0x06;

        case 0x40: return 0x02;
        case 0x41: return 0x00;
        case 0x42: return 0x10;
        case 0x43: return 0x00;

        case 0x44: return 0x03;
        case 0x45: return 0x02;
        case 0x46: return 0x00;
        case 0x47: return 0x1f;
    
        case 0x3c: return neon250->int_line; /* Interrupt line */
        case 0x3d: return PCI_INTA;          /* Interrupt pin */
    }

    neon250_log("NEON250: PCI read func=%d addr=%02X val=%02X\n", func, addr, ret);
    
    return ret;
}

/* Recalculate memory mappings based on PCI configuration */
static void 
neon250_recalc_mapping(neon250_t *neon250)
{
    if (!(neon250->pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_MEM)) {
        /* PCI memory access disabled */
        mem_mapping_disable(&neon250->mmio_mapping);
        mem_mapping_disable(&neon250->fb_mapping);
        mem_mapping_disable(&neon250->vga_mapping);
        return;
    }

    /* Set up MMIO mapping */
    if (neon250->mmio_base) {
        mem_mapping_set_addr(&neon250->mmio_mapping, neon250->mmio_base, 0x4000);
    } else {
        mem_mapping_disable(&neon250->mmio_mapping);
    }

    /* Set up framebuffer mapping */
    if (neon250->fb_base) {
        mem_mapping_set_addr(&neon250->fb_mapping, neon250->fb_base, neon250->vram_size);
    } else {
        mem_mapping_disable(&neon250->fb_mapping);
    }

    /* Set up VGA memory mapping if enabled */
    if (neon250->pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_MEM) {
        mem_mapping_enable(&neon250->vga_mapping);
    } else {
        mem_mapping_disable(&neon250->vga_mapping);
    }
}

/* PCI configuration write function */
static void
neon250_pci_write(int func, int addr, uint8_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    int offset;
    
    if (func > 0)
        return;
    
    neon250_log("NEON250: PCI write func=%d addr=%02X val=%02X\n", func, addr, val);
    
    offset = addr % 4;
    addr /= 4;
    
    switch (addr) {
        case 0x00: case 0x01: case 0x02: case 0x03:
            return; /* Read-only registers */
            
        case 0x04:
            neon250->pci_regs[0x04] = val & 0x37;
            io_removehandler(0x03c0, 0x0020, neon250_svga_in, NULL, NULL,
                    neon250_svga_out, NULL, NULL, neon250);
            if (val & PCI_COMMAND_IO)
                io_sethandler(0x03c0, 0x0020, neon250_svga_in, NULL, NULL,
                        neon250_svga_out, NULL, NULL, neon250);
            neon250_recalc_mapping(neon250);
            break;
            
        case 0x05: case 0x06:
            neon250->pci_regs[addr] &= ~(0xFF << (offset * 8));
            neon250->pci_regs[addr] |= (val << (offset * 8));
        break;
            
        case 0x07: case 0x08: case 0x09: case 0x0A: case 0x0B:
            return; /* Read-only registers */
            
        case 0x30: case 0x32:
        case 0x33:
            neon250->pci_regs[addr] = val;
            if (neon250->pci_regs[0x30] & 0x01) {
                uint32_t addr = (neon250->pci_regs[0x32] << 16) | (neon250->pci_regs[0x33] << 24);
                mem_mapping_set_addr(&neon250->bios_rom.mapping, addr, 0x20000);
            } else
                mem_mapping_disable(&neon250->bios_rom.mapping);
            break;   

        default:
            /* Handle base addresses and other registers */
            if (addr == 0x04) { /* MMIO base address */
                if (offset == 0) {
                    neon250->pci_regs[addr] &= ~0xFF;
                    neon250->pci_regs[addr] |= (val & 0xF0); /* 16-byte aligned, memory space */
                } else if (offset == 3) {
                    neon250->pci_regs[addr] &= ~0xFF000000;
                    neon250->pci_regs[addr] |= (val << 24);
                    neon250->mmio_base = (neon250->pci_regs[addr] & 0xFFFFFFF0);
                }
                neon250_recalc_mapping(neon250);
            } 
            else if (addr == 0x05) { /* Framebuffer base address */
                if (offset == 0) {
                    neon250->pci_regs[addr] &= ~0xFF;
                    neon250->pci_regs[addr] |= (val & 0xF0); /* 16-byte aligned, memory space */
                } else if (offset == 3) {
                    neon250->pci_regs[addr] &= ~0xFF000000;
                    neon250->pci_regs[addr] |= (val << 24);
                    neon250->fb_base = (neon250->pci_regs[addr] & 0xFFFFFFF0);
                }
                neon250_recalc_mapping(neon250);
            }
            else if (addr == 0x0F && offset == 0) { /* Interrupt line */
                neon250->pci_regs[addr] &= ~0xFF;
                neon250->pci_regs[addr] |= val;
                neon250->int_line = val;
            }
            else if (addr < (256/4)) {
                neon250->pci_regs[addr] &= ~(0xFF << (offset * 8));
                neon250->pci_regs[addr] |= (val << (offset * 8));
            }
            break;
    }
}
/* Initialize register defaults */
static void
neon250_init_registers(neon250_t *neon250)
{
    /* Set default values for core registers */
    memset(neon250->regs, 0, sizeof(neon250->regs));
    
    /* Setup the interrupt mask register - all interrupts masked */
    neon250->regs[NEON_REG_INTMASK] = 0x00000000;
    
    /* Default power state - powered on */
    neon250->regs[NEON_REG_POWERDOWN] = 0x00000000;
    
    /* Default texture control - no bilinear filtering, no dithering */
    neon250->regs[NEON_REG_TEXCTRL] = 0x00000000;
    
    /* Default fog control - fog disabled */
    neon250->regs[NEON_REG_FOGCTRL] = 0x00000000;
    
    /* Default fog color - medium gray */
    neon250->regs[NEON_REG_FOGCOLOR] = 0x00808080;
    
    /* Default render control - idle */
    neon250->regs[NEON_REG_RENDERCTRL] = 0x00000000;
    
    /* Reset rendering state */
    neon250->render_state = NEON250_RENDER_STATE_IDLE;
    
    neon250_log("NEON250: Registers initialized\n");
}

/* SVGA timing recalculation function for Neon 250 */
static void
neon250_svga_recalctimings(svga_t *svga)
{
    neon250_t *neon250 = (neon250_t *)svga->priv;
    
    neon250_log("NEON250: Recalculating SVGA timings\n");
    
    /* Base calculations from standard SVGA */
    svga_recalctimings(svga);
    
    /* PowerVR-specific overrides based on pvr2fb.c from Linux kernel */
    if (neon250->regs[NEON250_EXT_BPPCONTROL] & 0x01) {
        /* Enhanced mode enabled */
        uint32_t bpp_mode = (neon250->regs[NEON250_EXT_BPPCONTROL] >> 1) & 0x07;
        
        switch (bpp_mode) {
            case 0: svga->bpp = 8; break;  /* 8bpp (palette) */
            case 1: svga->bpp = 15; break; /* 15bpp (RGB555) */
            case 2: svga->bpp = 16; break; /* 16bpp (RGB565) */
            case 3: svga->bpp = 24; break; /* 24bpp (RGB888) */
            case 4: svga->bpp = 32; break; /* 32bpp (ARGB8888) */
            default: svga->bpp = 8; break;
        }
        
        /* Apply stride if set */
        if (neon250->regs[NEON250_EXT_STRIDE] > 0) {
            svga->rowoffset = neon250->regs[NEON250_EXT_STRIDE] / (svga->bpp >> 3);
        }
        
        /* Apply pixel clock setting from extended registers */
        if (neon250->regs[NEON250_EXT_PIXELCLOCK] > 0) {
            double pixclock_mhz = (double)neon250->regs[NEON250_EXT_PIXELCLOCK] / 100.0;
            if (pixclock_mhz > 0) {
                svga->clock = (int)(pixclock_mhz * 1000000);
            }
        }
    }
    
    /* Update display status based on active registers */
    svga->fullchange = changeframecount;
    svga->ma_latch = (neon250->regs[NEON_REG_FRAMEBASE] & 0x3FFFFF) >> 1;
    
    /* Handle hardware cursor if enabled */
    if (neon250->regs[NEON250_EXT_HWCURSOR_CTRL] & 0x01) {
        svga->hwcursor.ena = 1;
        svga->hwcursor.x = neon250->regs[NEON250_EXT_HWCURSOR_POS] & 0xFFFF;
        svga->hwcursor.y = neon250->regs[NEON250_EXT_HWCURSOR_POS] >> 16;
        svga->hwcursor.addr = neon250->regs[NEON250_EXT_HWCURSOR_ADDR] & neon250->vram_mask;
        svga->hwcursor.cur_xsize = 64;  /* Use the correct field names */
        svga->hwcursor.cur_ysize = 64;  /* Use the correct field names */
    } else {
        svga->hwcursor.ena = 0;
    }
}

/* SVGA port write handler */
static void
neon250_svga_out(uint16_t addr, uint8_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    svga_t *svga = neon250->svga;
    uint8_t index;
    
    if (!svga)
        return;
        
    neon250_log("NEON250: SVGA out addr=%04X val=%02X\n", addr, val);
    
    if ((addr >= 0x3D0 && addr <= 0x3DF) && !(svga->miscout & 1))
        addr += 0x10;
        
    switch (addr) {
        case 0x3D4: /* CRTC index */
            svga->crtcreg = val;
            return;
            
        case 0x3D5: /* CRTC data */
            index = svga->crtcreg;
            
            /* Check for Neon 250 extended registers */
            if (index >= 0x40 && index <= 0x5F) {
                /* Handle extended registers */
                switch (index) {
                    case NEON250_EXT_PIXELCLOCK:
                    case NEON250_EXT_BPPCONTROL:
                    case NEON250_EXT_HWCURSOR_CTRL:
                    case NEON250_EXT_STRIDE:
                        neon250->regs[index] = val;
                        svga_recalctimings(svga);
                        break;
                        
                    case NEON250_EXT_HWCURSOR_POS:
                    case NEON250_EXT_HWCURSOR_POS1:
                    case NEON250_EXT_HWCURSOR_POS2:
                    case NEON250_EXT_HWCURSOR_POS3:
                        /* Handle 32-bit cursor position (written as 4 bytes) */
                        {
                            int shift = 8 * (index - NEON250_EXT_HWCURSOR_POS);
                            neon250->regs[NEON250_EXT_HWCURSOR_POS] &= ~(0xFF << shift);
                            neon250->regs[NEON250_EXT_HWCURSOR_POS] |= (val << shift);
                            svga_recalctimings(svga);
                        }
                        break;
                        
                    case NEON250_EXT_HWCURSOR_ADDR:
                    case NEON250_EXT_HWCURSOR_ADDR1:
                    case NEON250_EXT_HWCURSOR_ADDR2:
                    case NEON250_EXT_HWCURSOR_ADDR3:
                        /* Handle 32-bit cursor address (written as 4 bytes) */
                        {
                            int shift = 8 * (index - NEON250_EXT_HWCURSOR_ADDR);
                            neon250->regs[NEON250_EXT_HWCURSOR_ADDR] &= ~(0xFF << shift);
                            neon250->regs[NEON250_EXT_HWCURSOR_ADDR] |= (val << shift);
                            svga_recalctimings(svga);
                        }
                        break;
                }
                return;
            }
            
            /* Let standard SVGA handler process normal registers */
            svga->crtc[index] = val; // Instead of svga_write_crtc
            
            /* Post-process critical registers that affect mode */
            switch (index) {
                case NEON250_CRTC_H_TOTAL:
                case NEON250_CRTC_H_DISPLAY:
                case NEON250_CRTC_H_SYNC_START:
                case NEON250_CRTC_H_SYNC_END:
                case NEON250_CRTC_V_TOTAL:
                case NEON250_CRTC_V_DISPLAY:
                case NEON250_CRTC_V_SYNC_START:
                case NEON250_CRTC_V_SYNC_END:
                case NEON250_CRTC_MODE_CONTROL:
                    svga_recalctimings(svga);
                    break;
            }
            return;
            
        default:
            /* Pass through to standard SVGA handler */
            svga_out(addr, val, svga);
            return;
    }
}

/* SVGA port read handler */
static uint8_t
neon250_svga_in(uint16_t addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    svga_t *svga = neon250->svga;
    uint8_t ret = 0xFF;
    
    if (!svga)
        return ret;
        
    if ((addr >= 0x3D0 && addr <= 0x3DF) && !(svga->miscout & 1))
        addr += 0x10;
        
    switch (addr) {
        case 0x3D4: /* CRTC index */
            ret = svga->crtcreg;
            break;
            
        case 0x3D5: /* CRTC data */
            if (svga->crtcreg >= 0x40 && svga->crtcreg <= 0x5F) {
                /* Handle extended registers specific to Neon 250 */
                switch (svga->crtcreg) {
                    case NEON250_EXT_PIXELCLOCK:
                    case NEON250_EXT_BPPCONTROL:
                    case NEON250_EXT_HWCURSOR_CTRL:
                    case NEON250_EXT_STRIDE:
                        ret = neon250->regs[svga->crtcreg];
                        break;
                        
                    case NEON250_EXT_HWCURSOR_POS:
                    case NEON250_EXT_HWCURSOR_POS1:
                    case NEON250_EXT_HWCURSOR_POS2:
                    case NEON250_EXT_HWCURSOR_POS3:
                        /* Handle 32-bit cursor position (read as 4 bytes) */
                        {
                            int shift = 8 * (svga->crtcreg - NEON250_EXT_HWCURSOR_POS);
                            ret = (neon250->regs[NEON250_EXT_HWCURSOR_POS] >> shift) & 0xFF;
                        }
                        break;
                        
                    case NEON250_EXT_HWCURSOR_ADDR:
                    case NEON250_EXT_HWCURSOR_ADDR1:
                    case NEON250_EXT_HWCURSOR_ADDR2:
                    case NEON250_EXT_HWCURSOR_ADDR3:
                        /* Handle 32-bit cursor address (read as 4 bytes) */
                        {
                            int shift = 8 * (svga->crtcreg - NEON250_EXT_HWCURSOR_ADDR);
                            ret = (neon250->regs[NEON250_EXT_HWCURSOR_ADDR] >> shift) & 0xFF;
                        }
                        break;
                        
                    default:
                        ret = 0xFF;
                        break;
                }
            } else {
                /* Standard CRTC registers */
                ret = svga->crtc[svga->crtcreg]; // Instead of svga_read_crtc
            }
            break;
            
        default:
            /* Pass through to standard SVGA handler */
            ret = svga_in(addr, svga);
            break;
    }
    
    neon250_log("NEON250: SVGA in addr=%04X val=%02X\n", addr, ret);
    return ret;
}

/* Linear framebuffer read for SVGA */
static uint8_t
neon250_svga_read_linear(uint32_t addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    svga_t *svga = neon250->svga;
    
    addr &= neon250->vram_mask;
    return neon250->vram[addr];
}

/* Linear framebuffer write for SVGA */
static void
neon250_svga_write_linear(uint32_t addr, uint8_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    svga_t *svga = neon250->svga;
    
    addr &= neon250->vram_mask;
    
    /* Check if we need to update screen */
    if (neon250->vram[addr] != val) {
        neon250->vram[addr] = val;
        // Use svga_changeeframecount instead of svga_mark_dirty
        svga->fullchange = changeframecount;
    }
}

/* Frame rendering function - process 3D commands */
static void
neon250_render_frame(neon250_t *neon250)
{
    /* In a real implementation, this would process 3D rendering commands */
    neon250_log("NEON250: Starting 3D rendering\n");
    
    /* Update rendering state */
    neon250->render_state = NEON250_RENDER_STATE_ACTIVE;
    
    /* Process 3D command buffer if 3D is enabled */
    if (neon250->state_3d && (neon250->regs[NEON_REG_RENDERCTRL] & 0x01)) {
        /* Start the 3D engine with the current command buffer */
        neon250->state_3d->control = 0x01;  /* Start rendering bit */
        neon250->state_3d->object_addr = neon250->regs[NEON_REG_OBJECTBASE];
        neon_3d_process_commands(neon250);
    }
    
    /* Update state when rendering is complete */
    neon250->render_state = NEON250_RENDER_STATE_COMPLETE;
    
    /* Update statistics */
    neon250->frames_rendered++;
    
    /* Set the render complete interrupt */
    neon250->regs[NEON_REG_INTSTATUS] |= 0x01;
    neon250_update_irq(neon250);
    
    neon250_log("NEON250: 3D rendering complete\n");
}

/* Framebuffer read function */
static uint8_t
neon250_fb_read(uint32_t addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    
    addr &= neon250->vram_mask;
    return neon250->vram[addr];
}

/* Framebuffer write function */
static void
neon250_fb_write(uint32_t addr, uint8_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    
    addr &= neon250->vram_mask;
    neon250->vram[addr] = val;
    
    /* Update the screen display */
    if (neon250->svga)
        svga_mark_dirty(neon250->svga, addr);
}

/* VGA memory read function */
static uint8_t
neon250_vram_read(uint32_t addr, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    
    return svga_read(addr, neon250->svga);
}

/* VGA memory write function */
static void
neon250_vram_write(uint32_t addr, uint8_t val, void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    
    svga_write(addr, val, neon250->svga);
}

/* Main initialization function */
static void *
neon250_init(const device_t *info)
{
    neon250_t *neon250 = (neon250_t *)malloc(sizeof(neon250_t));
    memset(neon250, 0, sizeof(neon250_t));
    
    neon250_log("NEON250: Initializing\n");
    
    /* Allocate VRAM - Neon 250 typically had 32MB of VRAM */
    neon250->vram_size = 32 * 1024 * 1024;
    neon250->vram = (uint8_t *)malloc(neon250->vram_size);
    neon250->vram_mask = neon250->vram_size - 1;
    memset(neon250->vram, 0, neon250->vram_size);
    
    /* Allocate texture memory - 8MB */
    neon250->texture_memory_size = 8 * 1024 * 1024;
    neon250->texture_memory = (uint8_t *)malloc(neon250->texture_memory_size);
    memset(neon250->texture_memory, 0, neon250->texture_memory_size);
    
    /* Initialize the SVGA device for 2D operations */
    video_inform(VIDEO_FLAG_TYPE_SPECIAL, &timing_neon250);
    
    /* Create SVGA device */
    neon250->svga = svga_get_pri();
    
    /* Set up SVGA capabilities */
    neon250->svga->priv = neon250;  /* Use 'priv' instead of 'p' */
    neon250->svga->vram = neon250->vram;
    neon250->svga->vram_max = neon250->vram_size;
    neon250->svga->vram_display_mask = neon250->vram_mask;
    
    /* Register SVGA callbacks */
    neon250->svga->recalctimings_ex = neon250_svga_recalctimings;  /* Use recalctimings_ex instead of recalctimings */
    
    /* Enable hardware cursor */
    neon250->svga->hwcursor.ena = 0;
    neon250->svga->hwcursor.cur_ysize = 64;  /* Use cur_ysize instead of ysize */
    neon250->svga->hwcursor.cur_xsize = 64;  /* Use cur_xsize instead of xsize */
    
    /* Initialize 3D engine */
    neon_3d_init(neon250);
    
    /* Load the BIOS ROM */
    rom_init(&neon250->bios_rom, "roms/video/pvr/n0020331.bin", 0xc0000, 0x20000, 0xffff,
            0, MEM_MAPPING_EXTERNAL);
    
    /* Set up mappings */
    mem_mapping_add(&neon250->mmio_mapping, 0, 0, 
                   NULL, NULL, neon250_reg_read,
                   NULL, NULL, neon250_reg_write,
                   NULL, MEM_MAPPING_EXTERNAL, neon250);
                   
    mem_mapping_add(&neon250->fb_mapping, 0, 0,
                   neon250_fb_read, NULL, NULL,
                   neon250_fb_write, NULL, NULL,
                   NULL, MEM_MAPPING_EXTERNAL, neon250);
                   
    mem_mapping_add(&neon250->vga_mapping, 0xa0000, 0x20000,
                   neon250_vram_read, NULL, NULL,
                   neon250_vram_write, NULL, NULL,
                   NULL, MEM_MAPPING_EXTERNAL, neon250);
    
    /* Initialize the PCI device */
    neon250->card_id = info->local;
    
    /* Setup PCI BAR configuration */
    neon250->pci_regs[PCI_REG_COMMAND] = 0x00;
    
    /* Add the card to the PCI bus */
    pci_add_card(PCI_ADD_NORMAL, neon250_pci_read, neon250_pci_write, neon250, &neon250->pci_slot);
    
    /* Disable mappings until configured by PCI */
    mem_mapping_disable(&neon250->mmio_mapping);
    mem_mapping_disable(&neon250->fb_mapping);
    mem_mapping_disable(&neon250->vga_mapping);
    
    /* Initialize registers */
    neon250_init_registers(neon250);
    
    /* Initialize the SVGA screen mode */
    if (neon250->svga) {
        svga_recalctimings(neon250->svga);
    }
    
    /* Setup timer for rendering operations with correct callback signature */
    timer_add(&neon250->render_timer, (void(*)(void*))neon250_render_frame, neon250, 0);
    
    neon250_log("NEON250: Initialization complete\n");
    
    return neon250;
}

/* Cleanup function */
static void
neon250_close(void *priv)
{
    neon250_t *neon250 = (neon250_t *)priv;
    
    neon250_log("NEON250: Closing\n");
    
    /* Free memory */
    if (neon250->vram)
        free(neon250->vram);
        
    if (neon250->texture_memory)
        free(neon250->texture_memory);
    
    /* Clean up 3D engine resources */
    if (neon250->state_3d)
        neon_3d_close(neon250);
        
    /* Free SVGA device */
    if (neon250->svga)
        svga_close(neon250->svga);
    
    free(neon250);
}

/* Device definition structure */
const device_t neon250_device = {
    .name = "VideoLogic PowerVR Neon 250",
    .internal_name = "neon250",
    .flags = DEVICE_AGP,
    .local = 0,
    .init = neon250_init,
    .close = neon250_close,
    .reset = NULL,
    .speed_changed = NULL,
    .config = NULL
};