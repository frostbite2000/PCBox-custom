/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          PowerVR PCX2 emulation core.
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

#include "86box/vid_powervr_pcx2.h"

#include "pcx2_render.h"  /* Include the 3D rendering pipeline header */

/* Include the reciprocal table and fog table */
#include "rectable.h"
#include "fogtable.h"

/* PCX2 Register Definitions - Based on PowerVR Series1 source code */
#define PCX_ID                0x000   /* Device ID Register */
#define PCX_REVISION          0x001   /* Revision Register */
#define PCX_SOFTRESET         0x002   /* Soft Reset Register */
#define PCX_INTSTATUS         0x003   /* Interrupt Status Register */
#define PCX_INTMASK           0x004   /* Interrupt Mask Register */
#define PCX_STARTRENDER       0x005   /* Start Rendering Register */
#define PCX_FOGAMOUNT         0x006   /* Fog Amount Register */
#define PCX_OBJECT_OFFSET     0x007   /* Object Offset Register */
#define PCX_PAGE_CTRL         0x008   /* Page Control Register */
#define PCX_ISP_BASE          0x00A   /* ISP Base Register */
#define PCX_PREC_BASE         0x00B   /* PREC Base Register */
#define PCX_TMEM_SETUP        0x00C   /* Texture Memory Setup Register */
#define PCX_TMEM_REFRESH      0x00D   /* Texture Memory Refresh Register */
#define PCX_FOGCOL            0x00E   /* Fog Color Register */
#define PCX_CAMERA            0x00F   /* Camera Register */
#define PCX_PACKMODE          0x010   /* Pack Mode Register */
#define PCX_ARBMODE           0x011   /* Arbiter Mode Register */
#define PCX_LSTRIDE           0x012   /* Line Stride Register */
#define PCX_SOFADDR           0x013   /* Start of Frame Address Register */
#define PCX_XCLIP             0x014   /* X Clip Register */
#define PCX_ABORTADDR         0x015   /* Abort Address Register */
#define PCX_GPPORT            0x016   /* General Purpose Port Register */

/* PCX2 Specific Registers */
#define PCX_IEEEFP            0x018   /* IEEE Floating Point Register */
#define PCX_BILINEAR          0x019   /* Bilinear Filtering Register */
#define PCX_PCI21COMP         0x01B   /* PCI 2.1 Compatibility Register */
#define PCX_CLKSELECT         0x01C   /* Clock Select Register */
#define PCX_FASTFOG           0x01D   /* Fast Fog Register */
#define PCX_POWERDOWN         0x01E   /* Power Down Register */

#define PCX_MEMTEST_DATA      0x07D   /* Memory Test Data Register */
#define PCX_MEMTEST_MODE      0x07E   /* Memory Test Mode Register */
#define PCX_MEMTEST_RES       0x07F   /* Memory Test Result Register */

#define PCX_FOG_TABLE         0x080   /* Fog Table Registers (0x080-0x0FF) */
#define PCX_TLB               0x100   /* Translation Lookaside Buffer (0x100-0x1FF) */
#define PCX_DIVIDER_TABLE     0x200   /* Divider Table Registers (0x200-0x3FF) */

/* PCX2 Device IDs */
#define PCX2_VENDOR_ID        0x1033  /* NEC vendor ID (PowerVR licensee) */
#define PCX2_DEVICE_ID        0x0046  /* PCX2 device ID */

/* PCX2 Texture Filtering Modes */
#define PCX2_FILTER_BILINEAR       0x00  /* Bilinear filtering */
#define PCX2_FILTER_ADAPTIVE       0x01  /* Adaptive bilinear filtering */
#define PCX2_FILTER_RESERVED       0x02  /* Reserved (set to 0) */
#define PCX2_FILTER_POINT_SAMPLE   0x03  /* Point sampling (linear mip-mapping) */

/* PCX2 Rendering States */
#define PCX2_RENDER_STATE_IDLE     0     /* No rendering in progress */
#define PCX2_RENDER_STATE_ACTIVE   1     /* Rendering in progress */
#define PCX2_RENDER_STATE_COMPLETE 2     /* Rendering completed */

/* PCX2 flags */
#define PCX2_FLAG_BILINEAR_ENABLE  0x01  /* Bilinear filtering enabled */
#define PCX2_FLAG_DITHER_ENABLE    0x02  /* Dithering enabled */
#define PCX2_FLAG_FOG_ENABLE       0x04  /* Fog enabled */
#define PCX2_FLAG_SDRAM_CACHING    0x08  /* SDRAM caching enabled */

/* PCX2 Device Struct */
typedef struct pcx2_t {
    uint32_t    pci_regs[256/4];      /* PCI configuration registers */
    uint32_t    regs[1024];           /* Hardware registers (DWORD resolution) */
    
    uint8_t    *vram;                 /* Video RAM buffer */
    uint32_t    vram_size;            /* Video RAM size (in bytes) */
    uint32_t    vram_mask;            /* Video RAM address mask */
    
    uint32_t    texture_memory_size;  /* Texture memory size (in bytes) */
    uint8_t    *texture_memory;       /* Texture memory buffer */
    uint8_t    *param_memory;         /* Parameter memory buffer */
    uint32_t    param_memory_size;    /* Parameter memory size (in bytes) */
    
    void       *render_context;       /* 3D rendering context */
    
    uint32_t    mmio_base, lfb_base;

    mem_mapping_t mmio_mapping;       /* Memory-mapped I/O region */
    mem_mapping_t linear_mapping;     /* Linear framebuffer mapping */
    
    uint8_t     pci_slot;             /* PCI slot number */
    int         card_id;              /* Unique card identifier */

    uint8_t		int_line;
    uint8_t     irq_state;
    
    /* Rendering state */
    int             render_state;       /* Current rendering state */
    uint32_t        flags;              /* Feature flags */
    
    /* Texture filtering settings */
    int             texture_filter_mode; /* Current texture filtering mode */
    
    /* Fog settings */
    uint32_t        fog_color;          /* Fog color (ARGB format) */
    uint32_t        fog_table[128];     /* Fog density table */
    uint32_t        fog_constant;       /* Fog constant value */
    
    /* Camera/projection settings */
    uint32_t        camera_scale;       /* Camera distance scale factor */
    float           camera_x_scale;     /* Camera X scale factor */
    float           camera_y_scale;     /* Camera Y scale factor */
    
    /* Performance monitoring */
    uint64_t        render_start_time;  /* Timestamp when rendering started */
    uint64_t        render_end_time;    /* Timestamp when rendering completed */
    uint32_t        frames_rendered;    /* Total frames rendered */
    uint32_t        render_timeouts;    /* Total rendering timeouts */

    pc_timer_t renderstarttimer;
    pc_timer_t renderendtimer;

    double renderstarttime;
    double renderendtime;
} pcx2_t;

#ifdef ENABLE_PCX2_LOG
int pcx2_do_log = ENABLE_PCX2_LOG;

static void
pcx2_log(const char *fmt, ...)
{
    va_list ap;
    
    if (pcx2_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
#define pcx2_log(fmt, ...)
#endif

/* Function prototypes */
void pcx2_render_start_poll(void *priv);
void pcx2_render_end_poll(void *priv);
static void     pcx2_update_irq(pcx2_t *pcx2);
static uint32_t pcx2_reg_read(uint32_t addr, void *priv);
static void     pcx2_reg_write(uint32_t addr, uint32_t val, void *priv);
static uint8_t  pcx2_pci_read(int func, int addr, void *priv);
static void     pcx2_pci_write(int func, int addr, uint8_t val, void *priv);
static void     pcx2_load_reciprocal_table(pcx2_t *pcx2);
static void     pcx2_load_fog_table(pcx2_t *pcx2);
static void     pcx2_set_fog_color(pcx2_t *pcx2, uint32_t color);
static void     pcx2_set_texture_filtering(pcx2_t *pcx2, int filter_mode);
static void     pcx2_set_dithering(pcx2_t *pcx2, bool enable);
static void     pcx2_set_fog_enable(pcx2_t *pcx2, bool enable);
static void     pcx2_start_render(pcx2_t *pcx2);
static bool     pcx2_finished_render(pcx2_t *pcx2);
static void     pcx2_set_x_clip(pcx2_t *pcx2, bool clip_left, int left, bool clip_right, int right);
static void     pcx2_init_core_registers(pcx2_t *pcx2);
static void     pcx2_process_3d_objects(pcx2_t *pcx2);

void
pcx2_render_start_poll(void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;

    pcx2->render_start_time = 0;

    timer_on_auto(&pcx2->renderstarttimer, pcx2->renderstarttime);
}

void
pcx2_render_end_poll(void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;

    pcx2->render_end_time = 0;

    timer_on_auto(&pcx2->renderendtimer, pcx2->renderendtime);
}

/* PCX2 Register Read Function */
static uint32_t
pcx2_reg_read(uint32_t addr, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    uint32_t offset = (addr & 0xFFF) >> 2;  /* Convert to DWORD offset */
    uint32_t ret = 0xFFFFFFFF;
    
    if (offset < 1024) {
        ret = pcx2->regs[offset];
        pcx2_log("PCX2: Read register 0x%03X = 0x%08X\n", offset, ret);
    }
    
    return ret;
}

/* PCX2 Register Write Function */
static void
pcx2_reg_write(uint32_t addr, uint32_t val, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    uint32_t offset = (addr & 0xFFF) >> 2;  /* Convert to DWORD offset */
    
    pcx2_log("PCX2: Write register 0x%03X = 0x%08X\n", offset, val);
    
    /* Handle special register writes */
    switch (offset) {
        case PCX_ID:
        case PCX_REVISION:
            /* Read-only registers, ignore writes */
            break;
            
        case PCX_SOFTRESET:
            /* Handle soft reset */
            if (val & 0x01) {
                pcx2_log("PCX2: Soft reset triggered\n");
                /* Reset device state here */
                pcx2->regs[PCX_INTSTATUS] = 0;
                pcx2_update_irq(pcx2);
            }
            break;
            
        case PCX_INTSTATUS:
            /* Writing to INTSTATUS clears the bits */
            pcx2->regs[offset] &= ~val;
            pcx2_update_irq(pcx2);
            break;
            
        case PCX_STARTRENDER:
            /* Trigger rendering operation */
            pcx2_log("PCX2: Start rendering operation\n");
            
            /* Start the 3D rendering process */
            pcx2->render_state = PCX2_RENDER_STATE_ACTIVE;
            timer_add(&pcx2->renderstarttimer, pcx2_render_start_poll, pcx2, 0);
            
            /* Process the 3D objects */
            pcx2_process_3d_objects(pcx2);
            break;
            
        case PCX_FOGCOL:
            /* Set fog color */
            pcx2->regs[offset] = val;
            pcx2->fog_color = val & 0x00FFFFFF;
            
            /* Update the rendering context with the new fog color */
            pcx2_set_render_fog_color(pcx2->render_context, 
                                     (pcx2->fog_color >> 16) & 0xFF,
                                     (pcx2->fog_color >> 8) & 0xFF,
                                     pcx2->fog_color & 0xFF);
            break;
            
        case PCX_BILINEAR:
            /* Set texture filtering mode */
            pcx2->regs[offset] = val;
            pcx2->texture_filter_mode = val & 0x03;
            
            /* Update the rendering context with the new texture filtering mode */
            pcx2_set_render_texture_filtering(pcx2->render_context, pcx2->texture_filter_mode);
            
            /* Update bilinear filtering flag */
            if (pcx2->texture_filter_mode == PCX2_FILTER_POINT_SAMPLE) {
                pcx2->flags &= ~PCX2_FLAG_BILINEAR_ENABLE;
            } else {
                pcx2->flags |= PCX2_FLAG_BILINEAR_ENABLE;
            }
            break;
            
        case PCX_PACKMODE:
            /* Set pack mode (including dithering) */
            pcx2->regs[offset] = val;
            
            /* Update the rendering context with the new dithering mode */
            pcx2_set_render_dithering(pcx2->render_context, (val & 0x10) ? 1 : 0);
            
            /* Update dithering flag */
            if (val & 0x10) {
                pcx2->flags |= PCX2_FLAG_DITHER_ENABLE;
            } else {
                pcx2->flags &= ~PCX2_FLAG_DITHER_ENABLE;
            }
            break;
            
        case PCX_POWERDOWN:
            /* Handle power down */
            pcx2_log("PCX2: Power %s\n", (val & 0x01) ? "down" : "up");
            pcx2->regs[offset] = val;
            break;
            
        default:
            /* Store the value in the register array */
            if (offset < 1024)
                pcx2->regs[offset] = val;
            break;
    }
}

/* PCX2 IRQ Update Function */
static void
pcx2_update_irq(pcx2_t *pcx2)
{
    uint32_t status = pcx2->regs[PCX_INTSTATUS];
    uint32_t mask = pcx2->regs[PCX_INTMASK];
    
    pci_set_irq(pcx2->pci_slot, PCI_INTA, &pcx2->irq_state);
}

/* PCX2 PCI Configuration Read Function */
static uint8_t
pcx2_pci_read(int func, int addr, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    uint8_t ret = 0xFF;
    
    if (func > 0)
        return ret;
    
    ret = pcx2->pci_regs[addr / 4] >> ((addr % 4) * 8);
    
    switch (addr) {
        case 0x00: return (PCX2_VENDOR_ID & 0xFF);
        case 0x01: return (PCX2_VENDOR_ID >> 8);
    
        case 0x02: return (PCX2_DEVICE_ID & 0xFF);
        case 0x03: return (PCX2_DEVICE_ID >> 8);
        
        case 0x04: return (PVR_PCI_COMMAND_L_MASTER); /*Respond to IO and memory accesses*/
        case 0x05: return 0x00;
    
        case 0x06: return (PVR_PCI_STATUS_L_66MHZ_CAPABLE);
        case 0x07: return (PVR_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING);
    
        case 0x08: return 0x01; /*Revision ID*/
        case 0x09: return 0x00;
    
        case 0x0a: return 0x00;
        case 0x0b: return 0x04;
        
        case 0x10: return 0x08;
        case 0x11: return 0x00;
        case 0x12: return 0x00;
        case 0x13: return pcx2->mmio_base >> 24;
    
        case 0x14: return 0x08;
        case 0x15: return 0x00;
        case 0x16: return 0x00;
        case 0x17: return pcx2->lfb_base >> 24;
    
        case 0x2c: case 0x2d: case 0x2e: case 0x2f:
            return pcx2->pci_regs[addr];

        case 0x3c: return pcx2->int_line;
        case 0x3d: return PCI_INTA;
    }

    pcx2_log("PCX2: PCI read func=%d addr=%02X val=%02X\n", func, addr, ret);
    
    return ret;
}

static void 
pcx2_recalc_mapping(pcx2_t *pcx2)
{
    if (!(pcx2->pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_MEM)) {
    //pclog("PCI mem off\n");
        mem_mapping_disable(&pcx2->mmio_mapping);
        mem_mapping_disable(&pcx2->linear_mapping);
    return;
    }

    //pclog("PCI mem on\n");
    //pclog("pcx2->mmio_base = %08X\n", pcx2->mmio_base);
    if (pcx2->mmio_base)
        mem_mapping_set_addr(&pcx2->mmio_mapping, pcx2->mmio_base, 0x10000);
    else
        mem_mapping_disable(&pcx2->mmio_mapping);

    //pclog("pcx2->lfb_base = %08X\n", pcx2->lfb_base);
    if (pcx2->lfb_base) {
    mem_mapping_set_addr(&pcx2->linear_mapping, pcx2->lfb_base, 0x400000);
    } else {
        mem_mapping_disable(&pcx2->linear_mapping);
    }
}

/* PCX2 PCI Configuration Write Function */
static void
pcx2_pci_write(int func, int addr, uint8_t val, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    uint32_t scratch;
    int offset;
    
    if (func > 0)
        return;
    
    pcx2_log("PCX2: PCI write func=%d addr=%02X val=%02X\n", func, addr, val);
    
    offset = addr % 4;
    addr /= 4;
    
    scratch = pcx2->pci_regs[addr];
    scratch &= ~(0xFF << (offset * 8));
    scratch |= val << (offset * 8);
    pcx2->pci_regs[addr] = scratch;
    
    switch (addr) {
    case 0x04:
        pcx2->pci_regs[0x04] = val;
        break;

    case 0x13:
        pcx2->mmio_base = val << 24;
        pcx2_recalc_mapping(pcx2);
        break;

    case 0x17: 
        pcx2->lfb_base = val << 24;
        pcx2_recalc_mapping(pcx2);
        break;
    }
}

/* PCX2 Linear Framebuffer Read Functions */
static uint8_t
pcx2_read_linear_byte(uint32_t addr, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    return pcx2->vram[addr];
}

static uint16_t
pcx2_read_linear_word(uint32_t addr, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    return *(uint16_t *)&pcx2->vram[addr];
}

static uint32_t
pcx2_read_linear_long(uint32_t addr, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    return *(uint32_t *)&pcx2->vram[addr];
}

/* PCX2 Linear Framebuffer Write Functions */
static void
pcx2_write_linear_byte(uint32_t addr, uint8_t val, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    pcx2->vram[addr] = val;
}

static void
pcx2_write_linear_word(uint32_t addr, uint16_t val, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    *(uint16_t *)&pcx2->vram[addr] = val;
}

static void
pcx2_write_linear_long(uint32_t addr, uint32_t val, void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    addr &= pcx2->vram_mask;
    *(uint32_t *)&pcx2->vram[addr] = val;
}

/* PCX2 Reciprocal Table Loading Function 
 * This loads the reciprocal table into the PCX2's on-chip RAM for fast division operations
 * Based on the original PowerVR Series1 implementation
 */
static void
pcx2_load_reciprocal_table(pcx2_t *pcx2)
{
    uint32_t i;
    
    pcx2_log("PCX2: Loading reciprocal table\n");
    
    /* Load the 512-entry reciprocal table into the PCX2 divider table registers */
    for (i = 0; i < 512; i++) {
        pcx2->regs[PCX_DIVIDER_TABLE + i] = pcx2_reciprocal_table[i];
    }
    
    pcx2_log("PCX2: Reciprocal table loaded\n");
}

/* PCX2 Fog Table Loading Function 
 * This loads the fog table into the PCX2's on-chip RAM for fog effects
 * Based on the original PowerVR Series1 implementation
 */
static void
pcx2_load_fog_table(pcx2_t *pcx2)
{
    uint32_t i;
    
    pcx2_log("PCX2: Loading fog table\n");
    
    /* Load the 128-entry fog table into the PCX2 fog table registers */
    for (i = 0; i < 128; i++) {
        pcx2->regs[PCX_FOG_TABLE + i] = pcx2_fog_table[i];
        pcx2->fog_table[i] = pcx2_fog_table[i];
    }
    
    pcx2_log("PCX2: Fog table loaded\n");
}

/* PCX2 Set Fog Color Function
 * Sets the color used for fog effects in the rendering pipeline
 * Based on the PowerVR Series1 TexasSetFogColour function
 */
static void
pcx2_set_fog_color(pcx2_t *pcx2, uint32_t color)
{
    uint32_t value;
    
    /* Convert ARGB color to hardware format */
    value = 0x1000000 | ((color >> 16) & 0xFF) << 16 | ((color >> 8) & 0xFF) << 8 | (color & 0xFF);
    
    /* Write the color to the fog color register */
    pcx2->regs[PCX_FOGCOL] = value;
    pcx2->fog_color = color;
    
    pcx2_log("PCX2: Set fog color to 0x%08X\n", color);
}

/* PCX2 Set Fog Enable Function
 * Enables or disables fog effects in the rendering pipeline
 */
static void
pcx2_set_fog_enable(pcx2_t *pcx2, bool enable)
{
    uint32_t value;
    
    /* Read current register value */
    value = pcx2->regs[PCX_FOGAMOUNT];
    
    if (enable) {
        /* Enable fog by setting the high bit */
        value |= 0x80000000;
        pcx2->flags |= PCX2_FLAG_FOG_ENABLE;
        pcx2_log("PCX2: Fog effects enabled\n");
    } else {
        /* Disable fog by clearing the high bit */
        value &= ~0x80000000;
        pcx2->flags &= ~PCX2_FLAG_FOG_ENABLE;
        pcx2_log("PCX2: Fog effects disabled\n");
    }
    
    /* Update the register */
    pcx2->regs[PCX_FOGAMOUNT] = value;
    
    /* Update the rendering context */
    if (pcx2->render_context) {
        pcx2_set_render_fog_enable(pcx2->render_context, enable ? 1 : 0);
    }
}

/* PCX2 Set Texture Filtering Mode Function
 * Configures the hardware texture filtering mode
 * Based on the PowerVR Series1 HWSetBilinearRegister function
 */
static void
pcx2_set_texture_filtering(pcx2_t *pcx2, int filter_mode)
{
    uint32_t value;
    
    /* Ensure the filter mode is valid */
    if (filter_mode < 0 || filter_mode > 3) {
        filter_mode = PCX2_FILTER_POINT_SAMPLE; /* Default to point sampling */
    }
    
    /* Read current register value to preserve other bits */
    value = pcx2->regs[PCX_BILINEAR];
    
    /* Clear the filtering bits (bits 0-1) */
    value &= ~0x03;
    
    /* Set the new filtering mode */
    value |= filter_mode;
    
    /* Update the register */
    pcx2->regs[PCX_BILINEAR] = value;
    pcx2->texture_filter_mode = filter_mode;
    
    /* Update the bilinear filtering flag */
    if (filter_mode == PCX2_FILTER_POINT_SAMPLE) {
        pcx2->flags &= ~PCX2_FLAG_BILINEAR_ENABLE;
        pcx2_log("PCX2: Texture filtering set to point sampling\n");
    } else {
        pcx2->flags |= PCX2_FLAG_BILINEAR_ENABLE;
        pcx2_log("PCX2: Texture filtering set to bilinear mode %d\n", filter_mode);
    }
    
    /* Update the rendering context */
    if (pcx2->render_context) {
        pcx2_set_render_texture_filtering(pcx2->render_context, filter_mode);
    }
}

/* PCX2 Set Dithering Function
 * Enables or disables color dithering when rendering at 16-bit color depth
 */
static void
pcx2_set_dithering(pcx2_t *pcx2, bool enable)
{
    uint32_t value;
    
    /* Read current pack mode register value */
    value = pcx2->regs[PCX_PACKMODE];
    
    if (enable) {
        /* Set dithering bit (bit 4) */
        value |= 0x10;
        pcx2->flags |= PCX2_FLAG_DITHER_ENABLE;
        pcx2_log("PCX2: Dithering enabled\n");
    } else {
        /* Clear dithering bit */
        value &= ~0x10;
        pcx2->flags &= ~PCX2_FLAG_DITHER_ENABLE;
        pcx2_log("PCX2: Dithering disabled\n");
    }
    
    /* Update the register */
    pcx2->regs[PCX_PACKMODE] = value;
    
    /* Update the rendering context */
    if (pcx2->render_context) {
        pcx2_set_render_dithering(pcx2->render_context, enable ? 1 : 0);
    }
}

/* PCX2 Start Render Function
 * Initiates the rendering process with the current parameters
 * Based on the PowerVR Series1 HWStartRender function
 */
static void
pcx2_start_render(pcx2_t *pcx2)
{
    /* Update rendering state */
    pcx2->render_state = PCX2_RENDER_STATE_ACTIVE;
    
    /* Record start time for performance monitoring */
    timer_add(&pcx2->renderstarttimer, pcx2_render_start_poll, pcx2, 0);
    
    /* Trigger the rendering process by writing to the start render register */
    pcx2->regs[PCX_STARTRENDER] = 1;
    
    /* In a real implementation, this would start the actual rendering process
     * For our emulation, we'll simulate it by setting up a timer to generate
     * an interrupt after a reasonable delay
     */
    pcx2_log("PCX2: Starting render operation\n");
    
    /* For now, we'll immediately set the operation as complete
     * In a more complete implementation, we'd use a timer to simulate
     * the actual rendering time
     */
    pcx2->regs[PCX_INTSTATUS] |= 0x01;  /* Set render complete bit */
    pcx2_update_irq(pcx2);
    
    /* Update rendering state */
    pcx2->render_state = PCX2_RENDER_STATE_COMPLETE;
    
    /* Record end time and update statistics */
    timer_add(&pcx2->renderendtimer, pcx2_render_end_poll, pcx2, 0);
    pcx2->frames_rendered++;
}

/* PCX2 Check Render Status Function
 * Checks if the current rendering operation has completed
 * Based on the PowerVR Series1 HWFinishedRender function
 */
static bool
pcx2_finished_render(pcx2_t *pcx2)
{
    /* Check if rendering is in progress */
    if (pcx2->render_state == PCX2_RENDER_STATE_ACTIVE) {
        /* Check status register to see if render is complete */
        if (pcx2->regs[PCX_INTSTATUS] & 0x01) {
            pcx2->render_state = PCX2_RENDER_STATE_COMPLETE;
            return true;
        }
        return false;
    }
    
    /* If we're not in active state, then rendering is either idle or already complete */
    return (pcx2->render_state == PCX2_RENDER_STATE_COMPLETE);
}

/* PCX2 Set X Clip Function
 * Sets the X-axis clipping region for rendering
 * Based on the PowerVR Series1 HWSetXClip function
 */
static void
pcx2_set_x_clip(pcx2_t *pcx2, bool clip_left, int left, bool clip_right, int right)
{
    uint32_t value = 0;
    
    /* Handle left clip */
    if (clip_left) {
        /* Ensure the value is within the valid range */
        if (left < 1) left = 1;
        if (left > 1023) left = 1023;
        
        value = (1 << 12) | left;
    }
    
    /* Handle right clip */
    if (clip_right) {
        /* Ensure the value is within the valid range */
        if (right < 1) right = 1;
        if (right > 1023) right = 1023;
        
        /* Ensure right >= left */
        if (clip_left && (right < left)) right = left;
        
        value |= (1 << 28) | (right << 16);
    }
    
    /* Update the X clip register */
    pcx2->regs[PCX_XCLIP] = value;
    
    pcx2_log("PCX2: Set X clip: left=%d (%s), right=%d (%s)\n",
            left, clip_left ? "enabled" : "disabled",
            right, clip_right ? "enabled" : "disabled");
}

/* PCX2 Init Core Registers Function
 * Initializes the core hardware registers to their default values
 * Based on the PowerVR Series1 HWSetupBunchOfISPRegs function
 */
static void
pcx2_init_core_registers(pcx2_t *pcx2)
{
    /* Set up the default register values */
    pcx2->regs[PCX_INTMASK] = 0x00000002;    /* Enable relevant interrupts */
    pcx2->regs[PCX_ISP_BASE] = 0x00080000;   /* ISP base address */
    pcx2->regs[PCX_PREC_BASE] = 0x00000000;  /* PREC base address */
    pcx2->regs[PCX_TMEM_REFRESH] = 0x00000480; /* TMEM refresh rate */
    pcx2->regs[PCX_XCLIP] = 0x00000000;      /* No X clipping by default */
    pcx2->regs[PCX_CLKSELECT] = 0x00000000;  /* Default clock selection */
    
    /* Enable IEEE floating point number support */
    pcx2->regs[PCX_IEEEFP] = 0x00000007;     /* Enable IEEE FP, edge scaling, and pointer concatenation */
    
    /* Enable PCI 2.1 compliance */
    pcx2->regs[PCX_PCI21COMP] = 0x00000000;
    
    /* Enable power down of internal RAM between renders */
    pcx2->regs[PCX_POWERDOWN] = 0x00000001;
    
    /* Set default texture memory configuration for 4MB */
    pcx2->regs[PCX_TMEM_SETUP] = 0x22212275; /* 4MB texture memory configuration */
    
    /* Set up default pixel format (16-bit RGB565) */
    pcx2->regs[PCX_PACKMODE] = 0x00000002;
    
    /* Set the default arbiter mode */
    pcx2->regs[PCX_ARBMODE] = 0x0000000D;    /* Optimal for PCX2 */
    
    /* Set default bilinear filtering mode (point sampling) */
    pcx2_set_texture_filtering(pcx2, PCX2_FILTER_POINT_SAMPLE);
    
    /* Set up default fog settings */
    pcx2_set_fog_color(pcx2, 0x00808080);    /* Medium gray fog color */
    pcx2->regs[PCX_FASTFOG] = 0x00000000;    /* Disable fast fog by default */
    
    /* Initialize rendering state */
    pcx2->render_state = PCX2_RENDER_STATE_IDLE;
    
    pcx2_log("PCX2: Core registers initialized\n");
}

/* PCX2 Process 3D Objects Function
 * Processes a list of 3D objects for rendering
 */
static void
pcx2_process_3d_objects(pcx2_t *pcx2)
{
    uint32_t object_offset, object_count;
    uint32_t *object_list;
    int i;
    
    /* Get the object list offset from the register */
    object_offset = pcx2->regs[PCX_OBJECT_OFFSET];
    
    /* The first DWORD at the object offset contains the number of objects */
    object_count = ((uint32_t *)pcx2->param_memory)[object_offset / 4];
    
    /* Limit the object count to a reasonable maximum to prevent crashes */
    if (object_count > 10000) {
        pcx2_log("PCX2: Warning - Object count %d exceeds maximum, clamping\n", object_count);
        object_count = 10000;
    }
    
    /* Get pointer to the actual object list (skip the count DWORD) */
    object_list = &((uint32_t *)pcx2->param_memory)[(object_offset / 4) + 1];
    
    pcx2_log("PCX2: Processing %d 3D objects\n", object_count);
    
    /* Start frame rendering using our 3D pipeline */
    pcx2_render_frame(pcx2->render_context, 
                     (pcx2_rgb_t *)pcx2->vram, 
                     pcx2->regs[PCX_LSTRIDE] / 4,  /* Stride in pixels (bytes/4) */
                     pcx2->vram_size / 4,          /* Height (approximate based on VRAM size) */
                     object_list, 
                     object_count);
                     
    /* Update rendering state */
    pcx2->render_state = PCX2_RENDER_STATE_COMPLETE;
    
    /* Record end time and update statistics */
    timer_add(&pcx2->renderendtimer, pcx2_render_end_poll, pcx2, 0);
    pcx2->frames_rendered++;
    
    /* Set the render complete interrupt */
    pcx2->regs[PCX_INTSTATUS] |= 0x01;
    pcx2_update_irq(pcx2);
}

/* PCX2 Initialization Function */
static void *
pcx2_init(const device_t *info)
{
    pcx2_t *pcx2 = (pcx2_t *)malloc(sizeof(pcx2_t));
    memset(pcx2, 0, sizeof(pcx2_t));
    
    pcx2_log("PCX2: Initializing\n");
    
    /* Allocate VRAM - PCX2 typically had 4MB of VRAM */
    pcx2->vram_size = 4 * 1024 * 1024;
    pcx2->vram = (uint8_t *)malloc(pcx2->vram_size);
    pcx2->vram_mask = pcx2->vram_size - 1;
    memset(pcx2->vram, 0, pcx2->vram_size);
    
    /* Allocate texture memory - PCX2 typically had 4MB of texture memory */
    pcx2->texture_memory_size = 4 * 1024 * 1024;
    pcx2->texture_memory = (uint8_t *)malloc(pcx2->texture_memory_size);
    memset(pcx2->texture_memory, 0, pcx2->texture_memory_size);
    
    /* Allocate parameter memory - PCX2 typically had 1MB of parameter memory */
    pcx2->param_memory_size = 1 * 1024 * 1024;
    pcx2->param_memory = (uint8_t *)malloc(pcx2->param_memory_size);
    memset(pcx2->param_memory, 0, pcx2->param_memory_size);
    
    /* Initialize hardware registers */
    pcx2->regs[PCX_ID] = 0x00000001;        /* Device ID */
    pcx2->regs[PCX_REVISION] = 0x00000002;  /* Revision 2 (PCX2) */
    
    /* Register PCI device */
    pcx2->card_id = device_get_instance();
    pci_add_card(PCI_ADD_NORMAL, pcx2_pci_read, pcx2_pci_write, pcx2, &pcx2->pci_slot);
    
    /* Initialize memory mappings */
    mem_mapping_add(&pcx2->mmio_mapping, 0, 0, 
                   (void *)pcx2_reg_read, NULL, NULL,
                   (void *)pcx2_reg_write, NULL, NULL,
                   NULL, MEM_MAPPING_EXTERNAL, pcx2);
    mem_mapping_disable(&pcx2->mmio_mapping);
    
    mem_mapping_add(&pcx2->linear_mapping, 0, 0,
                   pcx2_read_linear_byte, pcx2_read_linear_word, pcx2_read_linear_long,
                   pcx2_write_linear_byte, pcx2_write_linear_word, pcx2_write_linear_long,
                   NULL, MEM_MAPPING_EXTERNAL, pcx2);
    mem_mapping_disable(&pcx2->linear_mapping);
    
    /* Load reciprocal table */
    pcx2_load_reciprocal_table(pcx2);
    
    /* Load fog table */
    pcx2_load_fog_table(pcx2);
    
    /* Initialize core registers */
    pcx2_init_core_registers(pcx2);
    
    /* Initialize 3D rendering pipeline */
    pcx2_init_render_pipeline(pcx2, (uint32_t *)pcx2->texture_memory, (uint32_t *)pcx2->param_memory);
    
    /* Set default rendering parameters */
    pcx2_set_render_fog_color(pcx2->render_context, 
                             (pcx2->fog_color >> 16) & 0xFF,
                             (pcx2->fog_color >> 8) & 0xFF,
                             pcx2->fog_color & 0xFF);
    pcx2_set_render_texture_filtering(pcx2->render_context, pcx2->texture_filter_mode);
    pcx2_set_render_fog_enable(pcx2->render_context, (pcx2->flags & PCX2_FLAG_FOG_ENABLE) ? 1 : 0);
    pcx2_set_render_dithering(pcx2->render_context, (pcx2->flags & PCX2_FLAG_DITHER_ENABLE) ? 1 : 0);
    
    pcx2_log("PCX2: Initialization complete\n");
    
    return pcx2;
}

/* PCX2 Close Function */
static void
pcx2_close(void *priv)
{
    pcx2_t *pcx2 = (pcx2_t *)priv;
    
    pcx2_log("PCX2: Closing\n");
    
    /* Shutdown the 3D rendering pipeline */
    if (pcx2->render_context) {
        pcx2_shutdown_render_pipeline(pcx2->render_context);
        pcx2->render_context = NULL;
    }
    
    /* Free allocated memory */
    if (pcx2->vram) {
        free(pcx2->vram);
        pcx2->vram = NULL;
    }
    if (pcx2->texture_memory) {
        free(pcx2->texture_memory);
        pcx2->texture_memory = NULL;
    }
    if (pcx2->param_memory) {
        free(pcx2->param_memory);
        pcx2->param_memory = NULL;
    }
    
    /* Free device structure */
    free(pcx2);
}

/* PCX2 Device Definition */
const device_t pcx2_device = {
    .name          = "PowerVR PCX2",
    .internal_name = "powervr_pcx2",
    .flags         = DEVICE_PCI,
    .local         = 0,
    .init          = pcx2_init,
    .close         = pcx2_close,
    .reset         = NULL,
    .available = NULL,
    .speed_changed = NULL,
    .force_redraw  = NULL,
    .config        = NULL
};