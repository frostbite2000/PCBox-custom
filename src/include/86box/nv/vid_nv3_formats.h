/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3: Surface format definitions for RIVA 128
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000
 */

#ifndef NV3_FORMATS_H
#define NV3_FORMATS_H

// NV3 Context Surface Formats
#define NV3_CONTEXT_SURFACE_FORMAT_Y8         0x01  // 8-bit indexed
#define NV3_CONTEXT_SURFACE_FORMAT_X1R5G5B5   0x02  // 15-bit color
#define NV3_CONTEXT_SURFACE_FORMAT_R5G6B5     0x03  // 16-bit color
#define NV3_CONTEXT_SURFACE_FORMAT_A8R8G8B8   0x04  // 32-bit color with alpha
#define NV3_CONTEXT_SURFACE_FORMAT_X8R8G8B8   0x05  // 32-bit color without alpha

#endif // NV3_FORMATS_H