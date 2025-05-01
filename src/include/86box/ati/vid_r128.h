/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 * 
 *          Welcome to what happens when a single person demands that their overly complicated "vision" of a design be implemented 
 *          with absolutely no compromise. This is true lunacy.
 * 
 *          Explanation of what is being done here, and how this all works, hopefully posted on the 86Box blog.
 *          Notes specific to a subsystem in the header or c file for that subsystem
 *          Also check the doc folder for some more notres
 * 
 *          vid_r128.h:     Rage128 Architecture Hardware Reference (open-source)
 *          Last updated:   1 May 2025 (STILL WORKING ON IT!!!)
 *
 *			based on "programs/Xserver/hw/xfree86/vga256/drivers/r128/r128_reg.h" from xFree86 3.3.6
 *  
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000
 */

#pragma once

// The GPU base structure
extern const device_config_t r128_config[];

#define R128_MMIO_SIZE                                   0x1000000       // Max MMIO size

#define R128_REG_SIZE 									 0x4000			 // Max registers size (16K)

#define R128_LFB_MIRROR_START                            0x800000

#define R128_LFB_MAPPING_SIZE                            0x4000000       // size of prefetchable dfb

// various vbioses for testing
#define R128_VBIOS_RAGE128PRO_16MB                         "roms/video/ati/r128/rage128progl16mb.VBI"
#define R128_VBIOS_RAGE128PRO_32MB                         "roms/video/ati/r128/rage128pro32mb.BIN"

// The default VBIOS to use
#define R128_VBIOS_DEFAULT                               R128_VBIOS_RAGE128PRO_16MB

// Temporary, will be loaded from settings
#define R128_VRAM_SIZE_16MB                               0x1000000 // 16MB
#define R128_VRAM_SIZE_32MB                               0x2000000 // 32MB
#define R128_VRAM_SIZE_64MB                               0x4000000 // ATI Rage Fury MAXX only

// PCI config
#define R128_PCI_CFG_VENDOR_ID                           0x0
#define R128_PCI_CFG_DEVICE_ID                           0x2
#define R128_PCI_CFG_CAPABILITIES                        0x4

#define R128_PCI_COMMAND_L_IO                            1
#define R128_PCI_COMMAND_L_IO_ENABLED                    0x1
#define R128_PCI_COMMAND_L_MEMORY                        2
#define R128_PCI_COMMAND_L_MEMORY_ENABLED                0x1
#define R128_PCI_COMMAND_L_MASTER						 4
#define R128_PCI_COMMAND_L_SPECIAL						 8

#define R128_PCI_COMMAND_H_FAST_BACK2BACK                0x01

#define R128_PCI_STATUS_L_66MHZ_CAPABLE                  0x20
#define R128_PCI_STATUS_L_FAST_BACK						 0x80
#define R128_PCI_STATUS_L_CAP_LIST						 0x10
#define R128_PCI_STATUS_H_DEVSEL_TIMING                  5
#define R128_PCI_STATUS_H_FAST_DEVSEL_TIMING             0x00
#define R128_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING           0x02

#define R128_PCI_CFG_REVISION                            0x8

#define R128_PCI_CFG_PROGRAMMING_INTERFACE               0x9
#define R128_PCI_CFG_SUBCLASS_CODE                       0x0A
#define R128_PCI_CFG_CLASS_CODE                          0x0B
#define R128_PCI_CFG_CLASS_CODE_VGA                      0x03

#define R128_PCI_CFG_CACHE_LINE_SIZE                     0x0C
#define R128_PCI_CFG_CACHE_LINE_SIZE_DEFAULT_FROM_VBIOS  0x40

#define R128_PCI_CFG_LATENCY_TIMER                       0x0D
#define R128_PCI_CFG_HEADER_TYPE                         0x0E
#define R128_PCI_CFG_BIST                                0x0F

// PCI Bars
#define R128_PCI_CFG_BAR_PREFETCHABLE                    3
#define R128_PCI_CFG_BAR_PREFETCHABLE_ENABLED            0x1

#define R128_PCI_CFG_BAR0_L                              0x10
#define R128_PCI_CFG_BAR0_BYTE1                          0x11
#define R128_PCI_CFG_BAR0_BYTE2                          0x12
#define R128_PCI_CFG_BAR0_BASE_ADDRESS                   0x13
#define R128_PCI_CFG_BAR1_L                              0x14
#define R128_PCI_CFG_BAR1_BYTE1                          0x15
#define R128_PCI_CFG_BAR1_BYTE2                          0x16
#define R128_PCI_CFG_BAR1_BASE_ADDRESS                   0x17
#define R128_PCI_CFG_BAR2_L                  			 0x18
#define R128_PCI_CFG_BAR2_BYTE1                		     0x19
#define R128_PCI_CFG_BAR2_BYTE2                 		 0x1A
#define R128_PCI_CFG_BAR2_BASE_ADDRESS      			 0x1B
#define R128_PCI_CFG_SUBSYSTEM_ID                        0x2C

#define R128_PCI_CFG_ENABLE_VBIOS                        0x30
#define R128_PCI_CFG_VBIOS_BASE                          0x32 ... 0x33
#define R128_PCI_CFG_VBIOS_BASE_L                        0x32
#define R128_PCI_CFG_VBIOS_BASE_H                        0x33
#define R128_PCI_CFG_CAPPTR                 			 0x34

#define R128_PCI_CFG_INT_LINE                            0x3C
#define R128_PCI_CFG_INT_PIN                             0x3D

#define R128_PCI_CFG_SUBSYSTEM_ID_MIRROR_START           0x40
#define R128_PCI_CFG_SUBSYSTEM_ID_MIRROR_END             0x43

#define R128_PCI_CFG_MIN_GRANT                           0x3E
#define R128_PCI_CFG_MIN_GRANT_DEFAULT                   0x03
#define R128_PCI_CFG_MAX_LATENCY                         0x3F
#define R128_PCI_CFG_MAX_LATENCY_DEFAULT                 0x01

// GPU registers for Rage 128 for now

#define R128_ADAPTER_ID                   0x0f2c /* PCI */
#define R128_AGP_APER_OFFSET              0x0178
#define R128_AGP_BASE                     0x0170
#define R128_AGP_CNTL                     0x0174
#define R128_AGP_COMMAND                  0x0f58 /* PCI */
#define R128_AGP_PLL_CNTL                 0x0010 /* PLL */
#define R128_AGP_STATUS                   0x0f54 /* PCI */
#define R128_AMCGPIO_A_REG                0x01a0
#define R128_AMCGPIO_EN_REG               0x01a8
#define R128_AMCGPIO_MASK                 0x0194
#define R128_AMCGPIO_Y_REG                0x01a4
#define R128_ATTRDR                       0x03c1 /* VGA */
#define R128_ATTRDW                       0x03c0 /* VGA */
#define R128_ATTRX                        0x03c0 /* VGA */
#define R128_AUX_SC_CNTL                  0x1660
#define R128_AUX1_SC_BOTTOM               0x1670
#define R128_AUX1_SC_LEFT                 0x1664
#define R128_AUX1_SC_RIGHT                0x1668
#define R128_AUX1_SC_TOP                  0x166c
#define R128_AUX2_SC_BOTTOM               0x1680
#define R128_AUX2_SC_LEFT                 0x1674
#define R128_AUX2_SC_RIGHT                0x1678
#define R128_AUX2_SC_TOP                  0x167c
#define R128_AUX3_SC_BOTTOM               0x1690
#define R128_AUX3_SC_LEFT                 0x1684
#define R128_AUX3_SC_RIGHT                0x1688
#define R128_AUX3_SC_TOP                  0x168c

#define R128_BASE_CODE                    0x0f0b
#define R128_BIOS_0_SCRATCH               0x0010
#define R128_BIOS_1_SCRATCH               0x0014
#define R128_BIOS_2_SCRATCH               0x0018
#define R128_BIOS_3_SCRATCH               0x001c
#define R128_BIOS_ROM                     0x0f30 /* PCI */
#define R128_BIST                         0x0f0f /* PCI */
#define R128_BRUSH_DATA0                  0x1480
#define R128_BRUSH_DATA1                  0x1484
#define R128_BRUSH_DATA10                 0x14a8
#define R128_BRUSH_DATA11                 0x14ac
#define R128_BRUSH_DATA12                 0x14b0
#define R128_BRUSH_DATA13                 0x14b4
#define R128_BRUSH_DATA14                 0x14b8
#define R128_BRUSH_DATA15                 0x14bc
#define R128_BRUSH_DATA16                 0x14c0
#define R128_BRUSH_DATA17                 0x14c4
#define R128_BRUSH_DATA18                 0x14c8
#define R128_BRUSH_DATA19                 0x14cc
#define R128_BRUSH_DATA2                  0x1488
#define R128_BRUSH_DATA20                 0x14d0
#define R128_BRUSH_DATA21                 0x14d4
#define R128_BRUSH_DATA22                 0x14d8
#define R128_BRUSH_DATA23                 0x14dc
#define R128_BRUSH_DATA24                 0x14e0
#define R128_BRUSH_DATA25                 0x14e4
#define R128_BRUSH_DATA26                 0x14e8
#define R128_BRUSH_DATA27                 0x14ec
#define R128_BRUSH_DATA28                 0x14f0
#define R128_BRUSH_DATA29                 0x14f4
#define R128_BRUSH_DATA3                  0x148c
#define R128_BRUSH_DATA30                 0x14f8
#define R128_BRUSH_DATA31                 0x14fc
#define R128_BRUSH_DATA32                 0x1500
#define R128_BRUSH_DATA33                 0x1504
#define R128_BRUSH_DATA34                 0x1508
#define R128_BRUSH_DATA35                 0x150c
#define R128_BRUSH_DATA36                 0x1510
#define R128_BRUSH_DATA37                 0x1514
#define R128_BRUSH_DATA38                 0x1518
#define R128_BRUSH_DATA39                 0x151c
#define R128_BRUSH_DATA4                  0x1490
#define R128_BRUSH_DATA40                 0x1520
#define R128_BRUSH_DATA41                 0x1524
#define R128_BRUSH_DATA42                 0x1528
#define R128_BRUSH_DATA43                 0x152c
#define R128_BRUSH_DATA44                 0x1530
#define R128_BRUSH_DATA45                 0x1534
#define R128_BRUSH_DATA46                 0x1538
#define R128_BRUSH_DATA47                 0x153c
#define R128_BRUSH_DATA48                 0x1540
#define R128_BRUSH_DATA49                 0x1544
#define R128_BRUSH_DATA5                  0x1494
#define R128_BRUSH_DATA50                 0x1548
#define R128_BRUSH_DATA51                 0x154c
#define R128_BRUSH_DATA52                 0x1550
#define R128_BRUSH_DATA53                 0x1554
#define R128_BRUSH_DATA54                 0x1558
#define R128_BRUSH_DATA55                 0x155c
#define R128_BRUSH_DATA56                 0x1560
#define R128_BRUSH_DATA57                 0x1564
#define R128_BRUSH_DATA58                 0x1568
#define R128_BRUSH_DATA59                 0x156c
#define R128_BRUSH_DATA6                  0x1498
#define R128_BRUSH_DATA60                 0x1570
#define R128_BRUSH_DATA61                 0x1574
#define R128_BRUSH_DATA62                 0x1578
#define R128_BRUSH_DATA63                 0x157c
#define R128_BRUSH_DATA7                  0x149c
#define R128_BRUSH_DATA8                  0x14a0
#define R128_BRUSH_DATA9                  0x14a4
#define R128_BRUSH_SCALE                  0x1470
#define R128_BRUSH_Y_X                    0x1474
#define R128_BUS_CNTL                     0x0030
#define R128_BUS_CNTL1                    0x0034

#define R128_CACHE_CNTL                   0x1724
#define R128_CACHE_LINE                   0x0f0c /* PCI */
#define R128_CAP0_TRIG_CNTL               0x0950 /* ? */
#define R128_CAP1_TRIG_CNTL               0x09c0 /* ? */
#define R128_CAPABILITIES_ID              0x0f50 /* PCI */
#define R128_CAPABILITIES_PTR             0x0f34 /* PCI */
#define R128_CLK_PIN_CNTL                 0x0001 /* PLL */
#define R128_CLOCK_CNTL_DATA              0x000c
#define R128_CLOCK_CNTL_INDEX             0x0008
#       define R128_PLL_WR_EN             (1 << 7)
#       define R128_PLL_DIV_SEL           (3 << 8)
#define R128_CLR_CMP_CLR_3D               0x1a24
#define R128_CLR_CMP_CLR_DST              0x15c8
#define R128_CLR_CMP_CLR_SRC              0x15c4
#define R128_CLR_CMP_CNTL                 0x15c0
#define R128_CLR_CMP_MASK                 0x15cc
#define R128_CLR_CMP_MASK_3D              0x1A28
#define R128_COMMAND                      0x0f04 /* PCI */
#define R128_COMPOSITE_SHADOW_ID          0x1a0c
#define R128_CONFIG_APER_0_BASE           0x0100
#define R128_CONFIG_APER_1_BASE           0x0104
#define R128_CONFIG_APER_SIZE             0x0108
#define R128_CONFIG_BONDS                 0x00e8
#define R128_CONFIG_CNTL                  0x00e0
#define R128_CONFIG_MEMSIZE               0x00f8
#define R128_CONFIG_MEMSIZE_EMBEDDED      0x0114
#define R128_CONFIG_REG_1_BASE            0x010c
#define R128_CONFIG_REG_APER_SIZE         0x0110
#define R128_CONFIG_XSTRAP                0x00e4
#define R128_CONSTANT_COLOR_C             0x1d34
#define R128_CRC_CMDFIFO_ADDR             0x0740
#define R128_CRC_CMDFIFO_DOUT             0x0744
#define R128_CRTC_CRNT_FRAME              0x0214
#define R128_CRTC_DEBUG                   0x021c
#define R128_CRTC_EXT_CNTL                0x0054
#       define R128_CRTC_VGA_XOVERSCAN    (1 <<  0)
#       define R128_VGA_ATI_LINEAR        (1 <<  3)
#       define R128_XCRT_CNT_EN           (1 <<  6)
#       define R128_CRTC_HSYNC_DIS        (1 <<  8)
#       define R128_CRTC_VSYNC_DIS        (1 <<  9)
#       define R128_CRTC_DISPLAY_DIS      (1 << 10)
#define R128_CRTC_EXT_CNTL_DPMS_BYTE      0x0055
#       define R128_CRTC_HSYNC_DIS_BYTE   (1 <<  0)
#       define R128_CRTC_VSYNC_DIS_BYTE   (1 <<  1)
#       define R128_CRTC_DISPLAY_DIS_BYTE (1 <<  2)
#define R128_CRTC_GEN_CNTL                0x0050
#       define R128_CRTC_DBL_SCAN_EN      (1 <<  0)
#       define R128_CRTC_INTERLACE_EN     (1 <<  1)
#       define R128_CRTC_CUR_EN           (1 << 16)
#       define R128_CRTC_CUR_MODE_MASK    (7 << 17)
#       define R128_CRTC_EXT_DISP_EN      (1 << 24)
#       define R128_CRTC_EN               (1 << 25)
#       define R128_CRTC_PIX_WIDTH        (10 << 8)
#define R128_CRTC_PIX_WIDTH_PIXELMODE_4BPP 	      1
#define R128_CRTC_PIX_WIDTH_PIXELMODE_8BPP 	      2
#define R128_CRTC_PIX_WIDTH_PIXELMODE_15BPP 	  3
#define R128_CRTC_PIX_WIDTH_PIXELMODE_16BPP 	  4
#define R128_CRTC_PIX_WIDTH_PIXELMODE_24BPP 	  5
#define R128_CRTC_PIX_WIDTH_PIXELMODE_32BPP 	  6
#define R128_CRTC_GUI_TRIG_VLINE          0x0218
#define R128_CRTC_H_SYNC_STRT_WID         0x0204
#       define R128_CRTC_H_SYNC_POL       (1 << 23)
#define R128_CRTC_H_TOTAL_DISP            0x0200
#define R128_CRTC_OFFSET                  0x0224
#define R128_CRTC_OFFSET_CNTL             0x0228
#define R128_CRTC_PITCH                   0x022c
#define R128_CRTC_STATUS                  0x005c
#       define R128_CRTC_VBLANK_SAVE      (1 <<  1)
#define R128_CRTC_V_SYNC_STRT_WID         0x020c
#       define R128_CRTC_V_SYNC_POL       (1 << 23)
#define R128_CRTC_V_TOTAL_DISP            0x0208
#define R128_CRTC_VLINE_CRNT_VLINE        0x0210
#       define R128_CRTC_CRNT_VLINE_MASK  (0x7ff << 16)
#define R128_CRTC8_DATA                   0x03d5 /* VGA, 0x3b5 */
#define R128_CRTC8_IDX                    0x03d4 /* VGA, 0x3b4 */
#define R128_CUR_CLR0                     0x026c
#define R128_CUR_CLR1                     0x0270
#define R128_CUR_HORZ_VERT_OFF            0x0268
#define R128_CUR_HORZ_VERT_POSN           0x0264
#define R128_CUR_OFFSET                   0x0260
#       define R128_CUR_LOCK              (1 << 31)

#define R128_DAC_CNTL                     0x0058
#       define R128_DAC_RANGE_CNTL        (3 <<  0)
#       define R128_DAC_BLANKING          (1 <<  2)
#       define R128_DAC_8BIT_EN           (1 <<  8)
#       define R128_DAC_VGA_ADR_EN        (1 << 13)
#       define R128_DAC_MASK_ALL          (0xff << 24)
#define R128_DAC_CRC_SIG                  0x02cc
#define R128_DAC_DATA                     0x03c9 /* VGA */
#define R128_DAC_MASK                     0x03c6 /* VGA */
#define R128_DAC_R_INDEX                  0x03c7 /* VGA */
#define R128_DAC_W_INDEX                  0x03c8 /* VGA */
#define R128_DDA_CONFIG                   0x02e0
#define R128_DDA_ON_OFF                   0x02e4
#define R128_DEFAULT_OFFSET               0x16e0
#define R128_DEFAULT_PITCH                0x16e4
#define R128_DEFAULT_SC_BOTTOM_RIGHT      0x16e8
#       define R128_DEFAULT_SC_RIGHT_MAX  (0x1fff <<  0)
#       define R128_DEFAULT_SC_BOTTOM_MAX (0x1fff << 16)
#define R128_DESTINATION_3D_CLR_CMP_MSK   0x1824
#define R128_DESTINATION_3D_CLR_CMP_VAL   0x1820
#define R128_DEVICE_ID                    0x0f02 /* PCI */
#define R128_DP_BRUSH_BKGD_CLR            0x1478
#define R128_DP_BRUSH_FRGD_CLR            0x147c
#define R128_DP_CNTL                      0x16c0
#       define R128_DST_X_LEFT_TO_RIGHT   (1 <<  0)
#       define R128_DST_Y_TOP_TO_BOTTOM   (1 <<  1)
#define R128_DP_CNTL_XDIR_YDIR_YMAJOR     0x16d0
#define R128_DP_DATATYPE                  0x16c4
#define R128_DP_GUI_MASTER_CNTL           0x146c
#       define R128_GMC_SRC_PITCH_OFFSET_CNTL (1    <<  0)
#       define R128_GMC_DST_PITCH_OFFSET_CNTL (1    <<  1)
#       define R128_GMC_SRC_CLIPPING          (1    <<  2)
#       define R128_GMC_DST_CLIPPING          (1    <<  3)
#       define R128_GMC_BRUSH_DATATYPE_MASK   (0x0f <<  4)
#       define R128_GMC_BRUSH_8X8_MONO_FG_BG  (0    <<  4)
#       define R128_GMC_BRUSH_8X8_MONO_FG_LA  (1    <<  4)
#       define R128_GMC_BRUSH_1X8_MONO_FG_BG  (4    <<  4)
#       define R128_GMC_BRUSH_1X8_MONO_FG_LA  (5    <<  4)
#       define R128_GMC_BRUSH_1X8_COLOR       (12   <<  4)
#       define R128_GMC_BRUSH_SOLID_COLOR     (13   <<  4)
#       define R128_GMC_DST_DATATYPE_MASK     (0x0f <<  8)
#       define R128_GMC_DST_DATATYPE_SHIFT    8
#       define R128_GMC_SRC_DATATYPE_MASK       (3    << 12)
#       define R128_GMC_SRC_DATATYPE_MONO_FG_BG (0    << 12)
#       define R128_GMC_SRC_DATATYPE_MONO_FG_LA (1    << 12)
#       define R128_GMC_SRC_DATATYPE_COLOR      (3    << 12)
#       define R128_GMC_BYTE_PIX_ORDER        (1    << 14)
#       define R128_GMC_BYTE_LSB_TO_MSB       (1    << 14)
#       define R128_GMC_CONVERSION_TEMP       (1    << 15)
#       define R128_GMC_ROP3_MASK             (0xff << 16)
#       define R128_DP_SRC_SOURCE_MASK        (7    << 24)
#       define R128_DP_SRC_SOURCE_MEMORY      (2    << 24)
#       define R128_DP_SRC_SOURCE_HOST_DATA   (3    << 24)
#       define R128_GMC_3D_FCN_EN             (1    << 27)
#       define R128_GMC_CLR_CMP_CNTL_DIS      (1    << 28)
#       define R128_AUX_CLIP_DIS              (1    << 29)
#       define R128_GMC_WR_MSK_DS             (1    << 30)
#       define R128_GMC_LD_BRUSH_Y_X          (1    << 31)
#       define R128_ROP3_ZERO             0x00000000
#       define R128_ROP3_DSa              0x00880000
#       define R128_ROP3_SDna             0x00440000
#       define R128_ROP3_S                0x00cc0000
#       define R128_ROP3_DSna             0x00220000
#       define R128_ROP3_D                0x00aa0000
#       define R128_ROP3_DSx              0x00660000
#       define R128_ROP3_DSo              0x00ee0000
#       define R128_ROP3_DSon             0x00110000
#       define R128_ROP3_DSxn             0x00990000
#       define R128_ROP3_Dn               0x00550000
#       define R128_ROP3_SDno             0x00dd0000
#       define R128_ROP3_Sn               0x00330000
#       define R128_ROP3_DSno             0x00bb0000
#       define R128_ROP3_DSan             0x00770000
#       define R128_ROP3_ONE              0x00ff0000
#       define R128_ROP3_DPa              0x00a00000
#       define R128_ROP3_PDna             0x00500000
#       define R128_ROP3_P                0x00f00000
#       define R128_ROP3_DPna             0x000a0000
#       define R128_ROP3_D                0x00aa0000
#       define R128_ROP3_DPx              0x005a0000
#       define R128_ROP3_DPo              0x00fa0000
#       define R128_ROP3_DPon             0x00050000
#       define R128_ROP3_PDxn             0x00a50000
#       define R128_ROP3_PDno             0x00f50000
#       define R128_ROP3_Pn               0x000f0000
#       define R128_ROP3_DPno             0x00af0000
#       define R128_ROP3_DPan             0x005f0000


#define R128_DP_GUI_MASTER_CNTL_C         0x1c84
#define R128_DP_MIX                       0x16c8
#define R128_DP_SRC_BKGD_CLR              0x15dc
#define R128_DP_SRC_FRGD_CLR              0x15d8
#define R128_DP_WRITE_MASK                0x16cc
#define R128_DST_BRES_DEC                 0x1630
#define R128_DST_BRES_ERR                 0x1628
#define R128_DST_BRES_INC                 0x162c
#define R128_DST_BRES_LNTH                0x1634
#define R128_DST_BRES_LNTH_SUB            0x1638
#define R128_DST_HEIGHT                   0x1410
#define R128_DST_HEIGHT_WIDTH             0x143c
#define R128_DST_HEIGHT_WIDTH_8           0x158c
#define R128_DST_HEIGHT_WIDTH_BW          0x15b4
#define R128_DST_HEIGHT_Y                 0x15a0
#define R128_DST_OFFSET                   0x1404
#define R128_DST_PITCH                    0x1408
#define R128_DST_PITCH_OFFSET             0x142c
#define R128_DST_PITCH_OFFSET_C           0x1c80
#define R128_DST_WIDTH                    0x140c
#define R128_DST_WIDTH_HEIGHT             0x1598
#define R128_DST_WIDTH_X                  0x1588
#define R128_DST_WIDTH_X_INCY             0x159c
#define R128_DST_X                        0x141c
#define R128_DST_X_SUB                    0x15a4
#define R128_DST_X_Y                      0x1594
#define R128_DST_Y                        0x1420
#define R128_DST_Y_SUB                    0x15a8
#define R128_DST_Y_X                      0x1438

#define R128_EXT_MEM_CNTL                 0x0144

#define R128_FCP_CNTL                     0x0012 /* PLL */
#define R128_FLUSH_1                      0x1704
#define R128_FLUSH_2                      0x1708
#define R128_FLUSH_3                      0x170c
#define R128_FLUSH_4                      0x1710
#define R128_FLUSH_5                      0x1714
#define R128_FLUSH_6                      0x1718
#define R128_FLUSH_7                      0x171c

#define R128_GEN_INT_CNTL                 0x0040
#define R128_GEN_INT_STATUS               0x0044
#       define R128_VSYNC_INT_AK          (1 <<  2)
#       define R128_VSYNC_INT             (1 <<  2)
#define R128_GEN_RESET_CNTL               0x00f0
#       define R128_SOFT_RESET_GUI        (1 <<  0)
#define R128_GENENB                       0x03c3 /* VGA */
#define R128_GENFC_RD                     0x03ca /* VGA */
#define R128_GENFC_WT                     0x03da /* VGA, 0x03ba */
#define R128_GENMO_RD                     0x03cc /* VGA */
#define R128_GENMO_WT                     0x03c2 /* VGA */
#define R128_GENS0                        0x03c2 /* VGA */
#define R128_GENS1                        0x03da /* VGA, 0x03ba */
#define R128_GPIO_MONID                   0x0068
#define R128_GPIO_MONIDB                  0x006c
#define R128_GRPH8_DATA                   0x03cf /* VGA */
#define R128_GRPH8_IDX                    0x03ce /* VGA */
#define R128_GUI_DEBUG0                   0x16a0
#define R128_GUI_DEBUG1                   0x16a4
#define R128_GUI_DEBUG2                   0x16a8
#define R128_GUI_DEBUG3                   0x16ac
#define R128_GUI_DEBUG4                   0x16b0
#define R128_GUI_DEBUG5                   0x16b4
#define R128_GUI_DEBUG6                   0x16b8
#define R128_GUI_PROBE                    0x16bc
#define R128_GUI_SCRATCH_REG0             0x15e0
#define R128_GUI_SCRATCH_REG1             0x15e4
#define R128_GUI_SCRATCH_REG2             0x15e8
#define R128_GUI_SCRATCH_REG3             0x15ec
#define R128_GUI_SCRATCH_REG4             0x15f0
#define R128_GUI_SCRATCH_REG5             0x15f4
#define R128_GUI_STAT                     0x1740
#       define R128_GUI_FIFOCNT_MASK      0x0fff
#       define R128_GUI_ACTIVE            (1 << 31)

#define R128_HEADER                       0x0f0e /* PCI */
#define R128_HOST_DATA0                   0x17c0
#define R128_HOST_DATA1                   0x17c4
#define R128_HOST_DATA2                   0x17c8
#define R128_HOST_DATA3                   0x17cc
#define R128_HOST_DATA4                   0x17d0
#define R128_HOST_DATA5                   0x17d4
#define R128_HOST_DATA6                   0x17d8
#define R128_HOST_DATA7                   0x17dc
#define R128_HOST_DATA_LAST               0x17e0
#define R128_HOST_PATH_CNTL               0x0130
#define R128_HTOTAL_CNTL                  0x0009 /* PLL */
#define R128_HW_DEBUG                     0x0128

#define R128_I2C_CNTL_1                   0x0094 /* ? */
#define R128_INTERRUPT_LINE               0x0f3c /* PCI */
#define R128_INTERRUPT_PIN                0x0f3d /* PCI */
#define R128_IO_BASE                      0x0f14 /* PCI */

#define R128_LATENCY                      0x0f0d /* PCI */
#define R128_LEAD_BRES_DEC                0x1608
#define R128_LEAD_BRES_ERR                0x1600
#define R128_LEAD_BRES_INC                0x1604
#define R128_LEAD_BRES_LNTH               0x161c
#define R128_LEAD_BRES_LNTH_SUB           0x1624

#define R128_MAX_LATENCY                  0x0f3f /* PCI */
#define R128_MCLK_CNTL                    0x000f /* PLL */
#       define R128_FORCE_GCP             (1 << 16)
#       define R128_FORCE_PIPE3D_CPP      (1 << 17)
#define R128_MDGPIO_A_REG                 0x01ac
#define R128_MDGPIO_EN_REG                0x01b0
#define R128_MDGPIO_MASK                  0x0198
#define R128_MDGPIO_Y_REG                 0x01b4
#define R128_MEM_ADDR_CONFIG              0x0148
#define R128_MEM_BASE                     0x0f10 /* PCI */
#define R128_MEM_CNTL                     0x0140
#define R128_MEM_INIT_LAT_TIMER           0x0154
#define R128_MEM_INTF_CNTL                0x014c
#define R128_MEM_SDRAM_MODE_REG           0x0158
#define R128_MEM_STR_CNTL                 0x0150
#define R128_MEM_VGA_RP_SEL               0x003c
#define R128_MEM_VGA_WP_SEL               0x0038
#define R128_MIN_GRANT                    0x0f3e /* PCI */
#define R128_MISC_3D_STATE_CNTL_REG       0x1CA0
#define R128_MM_DATA                      0x0004
#define R128_MM_INDEX                     0x0000
#define R128_MPLL_CNTL                    0x000e /* PLL */
#define R128_MPP_TB_CONFIG                0x01c0 /* ? */
#define R128_MPP_GP_CONFIG                0x01c8 /* ? */

#define R128_N_VIF_COUNT                  0x0248

#define R128_OV0_SCALE_CNTL               0x0420 /* ? */
#define R128_OVR_CLR                      0x0230
#define R128_OVR_WID_LEFT_RIGHT           0x0234
#define R128_OVR_WID_TOP_BOTTOM           0x0238

#define R128_PALETTE_DATA                 0x00b4
#define R128_PALETTE_INDEX                0x00b0
#define R128_PC_DEBUG_MODE                0x1760
#define R128_PC_GUI_CTLSTAT               0x1748
#define R128_PC_GUI_MODE                  0x1744
#define R128_PC_NGUI_CTLSTAT              0x0184
#       define R128_PC_FLUSH_ALL          0x00ff
#       define R128_PC_BUSY               (1 << 31)
#define R128_PC_NGUI_MODE                 0x0180
#define R128_PCI_GART_PAGE                0x017c
#define R128_PLANE_3D_MASK_C              0x1d44
#define R128_PLL_TEST_CNTL                0x0013 /* PLL */
#define R128_PMI_CAP_ID                   0x0f5c /* PCI */
#define R128_PMI_DATA                     0x0f63 /* PCI */
#define R128_PMI_NXT_CAP_PTR              0x0f5d /* PCI */
#define R128_PMI_PMC_REG                  0x0f5e /* PCI */
#define R128_PMI_PMCSR_REG                0x0f60 /* PCI */
#define R128_PMI_REGISTER                 0x0f5c /* PCI */
#define R128_PPLL_CNTL                    0x0002 /* PLL */
#       define R128_PPLL_RESET                (1 <<  0)
#       define R128_PPLL_SLEEP                (1 <<  1)
#       define R128_PPLL_ATOMIC_UPDATE_EN     (1 << 16)
#       define R128_PPLL_VGA_ATOMIC_UPDATE_EN (1 << 17)
#define R128_PPLL_DIV_0                   0x0004 /* PLL */
#define R128_PPLL_DIV_1                   0x0005 /* PLL */
#define R128_PPLL_DIV_2                   0x0006 /* PLL */
#define R128_PPLL_DIV_3                   0x0007 /* PLL */
#       define R128_PPLL_FB3_DIV_MASK     0x07ff
#       define R128_PPLL_POST3_DIV_MASK   0x00070000
#define R128_PPLL_REF_DIV                 0x0003 /* PLL */
#       define R128_PPLL_REF_DIV_MASK     0x03ff
#       define R128_PPLL_ATOMIC_UPDATE_R  (1 << 15) /* same as _W */
#       define R128_PPLL_ATOMIC_UPDATE_W  (1 << 15) /* same as _R */
#define R128_PWR_MNGMT_CNTL_STATUS        0x0f60 /* PCI */
#define R128_REG_BASE                     0x0f18 /* PCI */
#define R128_REGPROG_INF                  0x0f09 /* PCI */
#define R128_REVISION_ID                  0x0f08 /* PCI */

#define R128_SC_BOTTOM                    0x164c
#define R128_SC_BOTTOM_RIGHT              0x16f0
#define R128_SC_BOTTOM_RIGHT_C            0x1c8c
#define R128_SC_LEFT                      0x1640
#define R128_SC_RIGHT                     0x1644
#define R128_SC_TOP                       0x1648
#define R128_SC_TOP_LEFT                  0x16ec
#define R128_SC_TOP_LEFT_C                0x1c88
#define R128_SEQ8_DATA                    0x03c5 /* VGA */
#define R128_SEQ8_IDX                     0x03c4 /* VGA */
#define R128_SNAPSHOT_F_COUNT             0x0244
#define R128_SNAPSHOT_VH_COUNTS           0x0240
#define R128_SNAPSHOT_VIF_COUNT           0x024c
#define R128_SRC_OFFSET                   0x15ac
#define R128_SRC_PITCH                    0x15b0
#define R128_SRC_PITCH_OFFSET             0x1428
#define R128_SRC_SC_BOTTOM                0x165c
#define R128_SRC_SC_BOTTOM_RIGHT          0x16f4
#define R128_SRC_SC_RIGHT                 0x1654
#define R128_SRC_X                        0x1414
#define R128_SRC_X_Y                      0x1590
#define R128_SRC_Y                        0x1418
#define R128_SRC_Y_X                      0x1434
#define R128_STATUS                       0x0f06 /* PCI */
#define R128_SUBPIC_CNTL                  0x0540 /* ? */
#define R128_SUB_CLASS                    0x0f0a /* PCI */
#define R128_SURFACE_DELAY                0x0b00
#define R128_SURFACE0_INFO                0x0b0c
#define R128_SURFACE0_LOWER_BOUND         0x0b04
#define R128_SURFACE0_UPPER_BOUND         0x0b08
#define R128_SURFACE1_INFO                0x0b1c
#define R128_SURFACE1_LOWER_BOUND         0x0b14
#define R128_SURFACE1_UPPER_BOUND         0x0b18
#define R128_SURFACE2_INFO                0x0b2c
#define R128_SURFACE2_LOWER_BOUND         0x0b24
#define R128_SURFACE2_UPPER_BOUND         0x0b28
#define R128_SURFACE3_INFO                0x0b3c
#define R128_SURFACE3_LOWER_BOUND         0x0b34
#define R128_SURFACE3_UPPER_BOUND         0x0b38
#define R128_SW_SEMAPHORE                 0x013c

#define R128_TEST_DEBUG_CNTL              0x0120
#define R128_TEST_DEBUG_MUX               0x0124
#define R128_TEST_DEBUG_OUT               0x012c
#define R128_TRAIL_BRES_DEC               0x1614
#define R128_TRAIL_BRES_ERR               0x160c
#define R128_TRAIL_BRES_INC               0x1610
#define R128_TRAIL_X                      0x1618
#define R128_TRAIL_X_SUB                  0x1620
#define R128_VCLK_ECP_CNTL                0x0008 /* PLL */
#define R128_VENDOR_ID                    0x0f00 /* PCI */
#define R128_VGA_DDA_CONFIG               0x02e8
#define R128_VGA_DDA_ON_OFF               0x02ec
#define R128_VID_BUFFER_CONTROL           0x0900
#define R128_VIDEOMUX_CNTL                0x0190
#define R128_VIPH_CONTROL                 0x01D0 /* ? */

#define R128_WAIT_UNTIL                   0x1720

#define R128_X_MPLL_REF_FB_DIV            0x000a /* PLL */
#define R128_XCLK_CNTL                    0x000d /* PLL */
#define R128_XDLL_CNTL                    0x000c /* PLL */
#define R128_XPLL_CNTL                    0x000b /* PLL */

#define R128_SCALE_3D_CNTL                0x1a00

// CRTC/CIO (0x3b0-0x3df)

#define R128_CRTC_REGISTER_INDEX_MONO                    0x3B4    
#define R128_CRTC_REGISTER_MONO                          0x3B5    // Currently Selected CRTC Register - Monochrome 

#define R128_CRTC_DATA_OUT                               0x3C0
#define R128_CRTC_MISCOUT                                0x3C2

#define R128_CRTC_REGISTER_INDEX                         0x3D4 
#define R128_CRTC_REGISTER_CURRENT                       0x3D5

// These are standard (0-18h)
#define R128_CRTC_REGISTER_HTOTAL                        0x00
#define R128_CRTC_REGISTER_HDISPEND                      0x01
#define R128_CRTC_REGISTER_HBLANKSTART                   0x02
#define R128_CRTC_REGISTER_HBLANKEND                     0x03
#define R128_CRTC_REGISTER_HRETRACESTART                 0x04
#define R128_CRTC_REGISTER_HRETRACEEND                   0x05
#define R128_CRTC_REGISTER_VTOTAL                        0x06
#define R128_CRTC_REGISTER_OVERFLOW                      0x07
#define R128_CRTC_REGISTER_PRESETROWSCAN                 0x08
#define R128_CRTC_REGISTER_MAXSCAN                       0x09
#define R128_CRTC_REGISTER_CURSOR_START                  0x0A
#define R128_CRTC_REGISTER_CURSOR_START_DISABLED         5
#define R128_CRTC_REGISTER_CURSOR_END                    0x0B
#define R128_CRTC_REGISTER_STARTADDR_HIGH                0x0C
#define R128_CRTC_REGISTER_STARTADDR_LOW                 0x0D
#define R128_CRTC_REGISTER_CURSORLOCATION_HIGH           0x0E
#define R128_CRTC_REGISTER_CURSORLOCATION_LOW            0x0F
#define R128_CRTC_REGISTER_VRETRACESTART                 0x10
#define R128_CRTC_REGISTER_VRETRACEEND                   0x11
#define R128_CRTC_REGISTER_VDISPEND                      0x12
#define R128_CRTC_REGISTER_OFFSET                        0x13
#define R128_CRTC_REGISTER_UNDERLINELOCATION             0x14
#define R128_CRTC_REGISTER_STARTVBLANK                   0x15
#define R128_CRTC_REGISTER_ENDVBLANK                     0x16
#define R128_CRTC_REGISTER_CRTCCONTROL                   0x17
#define R128_CRTC_REGISTER_LINECOMP                      0x18
#define R128_CRTC_REGISTER_STANDARDVGA_END               0x18

#define R128_CRTC_BANKED_128K_A0000                      0x00
#define R128_CRTC_BANKED_64K_A0000                       0x04
#define R128_CRTC_BANKED_32K_B0000                       0x08
#define R128_CRTC_BANKED_32K_B8000                       0x0C




typedef struct r128_pci_config_s
{
    uint8_t pci_regs[ATI_PCI_NUM_CFG_REGS];  // The actual pci register values (not really used, just so they can be stored - they aren't very good for code readability)
    bool    vbios_enabled;
    uint8_t int_line;
} r128_pci_config_t;

typedef struct r128_s
{
    ati_base_t atibase;   // Base ATi structure
    
    // Config
    r128_pci_config_t pci_config;    // PCI Configuration

    uint32_t config_cntl;
    uint32_t config_memsize;

    uint32_t context_load_cntl;
    uint32_t context_mask;

    uint32_t crtc_gen_cntl;
    uint8_t  crtc_ext_cntl;
    uint32_t crtc_h_sync_strt_wid;
    uint32_t crtc_h_total_disp;
    uint32_t crtc_v_sync_strt_wid;
    uint32_t crtc_v_total_disp;
    uint32_t crtc_off_pitch;

    uint32_t clock_cntl;

    uint32_t clr_cmp_clr;
    uint32_t clr_cmp_cntl;
    uint32_t clr_cmp_mask;

    uint32_t cur_horz_vert_off;
    uint32_t cur_horz_vert_posn;
    uint32_t cur_offset;

    uint32_t dac_cntl;

    uint32_t dp_bkgd_clr;
    uint32_t dp_frgd_clr;
    uint32_t dp_mix;
    uint32_t dp_pix_width;
    uint32_t dp_src;

    uint32_t dst_bres_lnth;
    uint32_t dst_bres_dec;
    uint32_t dst_bres_err;
    uint32_t dst_bres_inc;

    uint32_t dst_cntl;
    uint32_t dst_height_width;
    uint32_t dst_off_pitch;
    uint32_t dst_y_x;

    uint32_t gen_test_cntl;

    uint32_t gui_traj_cntl;

    uint32_t host_cntl;

    uint32_t mem_cntl;

    uint32_t ovr_clr;
    uint32_t ovr_wid_left_right;
    uint32_t ovr_wid_top_bottom;

    uint32_t pat_cntl;
    uint32_t pat_reg0;
    uint32_t pat_reg1;

    uint32_t sc_left_right;
    uint32_t sc_top_bottom;

    uint32_t scratch_reg0;
    uint32_t scratch_reg1;

    uint32_t src_cntl;
    uint32_t src_off_pitch;
    uint32_t src_y_x;
    uint32_t src_y_x_start;
    uint32_t src_height1_width1;
    uint32_t src_height2_width2;

    uint32_t write_mask;
    uint32_t chain_mask;

} r128_t;

// device object
extern r128_t* r128;

// Device Core
void*       r128_init(const device_t *info);
void        r128_close(void* priv);
void        r128_speed_changed(void *priv);
void        r128_draw_cursor(svga_t* svga, int32_t drawline);
void        r128_recalc_timings(svga_t* svga);
void        r128_force_redraw(void* priv);

/* BAR0 GPU MMIO read */
void        r128_update_mappings(void);                                          // Update memory mappings
uint8_t     r128_mmio_read8(uint32_t addr, void* priv);                          // Read 8-bit MMIO
uint16_t    r128_mmio_read16(uint32_t addr, void* priv);                         // Read 16-bit MMIO
uint32_t    r128_mmio_read32(uint32_t addr, void* priv);                         // Read 32-bit MMIO
void        r128_mmio_write8(uint32_t addr, uint8_t val, void* priv);            // Write 8-bit MMIO
void        r128_mmio_write16(uint32_t addr, uint16_t val, void* priv);          // Write 16-bit MMIO
void        r128_mmio_write32(uint32_t addr, uint32_t val, void* priv);          // Write 32-bit MMIO

/* BAR1 Dumb Framebuffer Read */
uint8_t     r128_dfb_read8(uint32_t addr, void* priv);                           // Write 8-bit DFB
uint16_t    r128_dfb_read16(uint32_t addr, void* priv);                          // Write 16-bit DFB
uint32_t    r128_dfb_read32(uint32_t addr, void* priv);                          // Write 32-bit DFB
void        r128_dfb_write8(uint32_t addr, uint8_t val, void* priv);             // Write 8-bit DFB
void        r128_dfb_write16(uint32_t addr, uint16_t val, void* priv);           // Write 16-bit DFB
void        r128_dfb_write32(uint32_t addr, uint32_t val, void* priv);           // Write 32-bit DFB

uint8_t     r128_svga_read(uint16_t addr, void* priv);                           // Read SVGA compatibility registers
void        r128_svga_write(uint16_t addr, uint8_t val, void* priv);             // Write SVGA registers
uint8_t     r128_pci_read(int32_t func, int32_t addr, void* priv);               // Read PCI configuration registers
void        r128_pci_write(int32_t func, int32_t addr, uint8_t val, void* priv); // Write PCI configuration registers

uint8_t 	r128_ext_in(uint16_t addr, void *priv);
void 		r128_ext_out(uint16_t addr, uint8_t val, void *priv);

// MMIO Arbitration
uint32_t    r128_mmio_arbitrate_read(uint32_t address);
void        r128_mmio_arbitrate_write(uint32_t address, uint32_t value);