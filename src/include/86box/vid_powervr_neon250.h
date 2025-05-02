/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          VideoLogic PowerVR Neon 250 emulation header.
 *
 * Authors: frostbite2000,
 *
 *          Copyright 2025 frostbite2000.
 */

 #ifndef VIDEO_POWERVR_NEON250_H
 #define VIDEO_POWERVR_NEON250_H
 
#include <86box/log.h>
#include <86box/i2c.h>
#include <86box/vid_ddc.h>
#include <86box/timer.h>
#include <86box/vid_svga.h>
#include <86box/86box.h>
#include <86box/video.h>
#include <86box/vid_svga_render.h>
#include <86box/rom.h>
 
 /* Forward declaration of 3D engine state */
 typedef struct neon_3d_state_t neon_3d_state_t;
 
 /* Forward declaration for the device struct */
 typedef struct neon250_t {
     uint32_t    pci_regs[256/4];      /* PCI configuration registers */
     uint32_t    regs[256];            /* Hardware registers */
     
     uint8_t    *vram;                 /* Video RAM buffer */
     uint32_t    vram_size;            /* Video RAM size (in bytes) */
     uint32_t    vram_mask;            /* Video RAM address mask */
     
     uint32_t    texture_memory_size;  /* Texture memory size (in bytes) */
     uint8_t    *texture_memory;       /* Texture memory buffer */
     
     uint32_t    mmio_base;            /* MMIO base address */
     uint32_t    fb_base;              /* Framebuffer base address */
     
     mem_mapping_t mmio_mapping;       /* Memory-mapped I/O region */
     mem_mapping_t fb_mapping;         /* Framebuffer mapping */
     mem_mapping_t vga_mapping;        /* VGA memory mapping */
     
     uint8_t     pci_slot;             /* PCI slot number */
     int         card_id;              /* Unique card identifier */
     
     uint8_t     int_line;             /* Interrupt line */
     uint8_t     irq_state;            /* IRQ state */
     
     int         render_state;         /* Current rendering state */
     uint32_t    flags;                /* Feature flags */
     
     uint32_t    fog_color;            /* Fog color (RGB format) */
     
     svga_t     *svga;                 /* SVGA device for 2D operations */
     
     rom_t       bios_rom;             /* BIOS ROM */
     
     pc_timer_t  render_timer;         /* Timer for rendering operations */
     
     /* Performance monitoring */
     uint32_t    frames_rendered;      /* Total frames rendered */
     
     /* 3D rendering state */
     neon_3d_state_t *state_3d;        /* 3D engine state */
 } neon250_t;
 
 /* Neon 250 device extended registers */
 #define NEON250_EXT_PIXELCLOCK    0x40
 #define NEON250_EXT_BPPCONTROL    0x41
 #define NEON250_EXT_HWCURSOR_CTRL 0x42
 #define NEON250_EXT_HWCURSOR_POS  0x43
 #define NEON250_EXT_HWCURSOR_ADDR 0x44
 #define NEON250_EXT_STRIDE        0x45
 
 /* Video modes */
 enum {
     NEON250_MODE_VGA = 0,
     NEON250_MODE_2D  = 1,
     NEON250_MODE_3D  = 2
 };
 
/* 3D engine functions */
void neon_3d_init(neon250_t *neon250);
void neon_3d_reset(neon250_t *neon250);
void neon_3d_close(neon250_t *neon250);
uint32_t neon_3d_read(neon250_t *neon250, uint32_t addr);
void neon_3d_write(neon250_t *neon250, uint32_t addr, uint32_t value);
void neon_3d_process_commands(neon250_t *neon250);
 
 /* SVGA helper functions */
 void neon250_hwcursor_draw(svga_t *svga, int displine);
 void neon250_setup_hwcursor(neon250_t *neon250);
 void neon250_calc_mode(neon250_t *neon250, int width, int height, int bpp);
 
 /* Device declaration */
 extern const device_t neon250_device;
 
 #endif /* VIDEO_POWERVR_NEON250_H */