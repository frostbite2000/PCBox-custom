/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          ATI Rage II+ DVD (based on Mach64 architecture) graphics card emulation header.
 *
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000.
 */

#ifndef VIDEO_ATI_RAGEIIDVD_H
#define VIDEO_ATI_RAGEIIDVD_H

// Chip ID for Rage II+ DVD is 0x4755 (from MAME code)
#define ATI_RAGEIIDVD_PCI_DEVICE_ID 0x4755

// Register offsets (from MAME's atirage.cpp)
#define CRTC_H_TOTAL_DISP  0x00
#define CRTC_V_TOTAL_DISP  0x08
#define CRTC_OFF_PITCH     0x14
#define CRTC_GEN_CNTL      0x1c
#define GP_IO              0x78
#define CLOCK_CNTL         0x90
#define CRTC_DAC_BASE      0xc0
#define CONFIG_CHIP_ID     0xe0

// PLL register offsets
#define PLL_MACRO_CNTL     1
#define PLL_REF_DIV        2
#define PLL_GEN_CNTL       3
#define MCLK_FB_DIV        4
#define PLL_VCLK_CNTL      5
#define VCLK_POST_DIV      6
#define VCLK0_FB_DIV       7
#define VCLK1_FB_DIV       8
#define VCLK2_FB_DIV       9
#define VCLK3_FB_DIV       10
#define PLL_XCLK_CNTL      11
#define PLL_FCP_CNTL       12

#endif /* VIDEO_ATI_RAGEIIDVD_H */