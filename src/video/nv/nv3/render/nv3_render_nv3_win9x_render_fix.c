/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Fix for Windows ME/2000 taskbar icon rendering in RIVA 128 emulation
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <86box/86box.h>
#include <86box/device.h>
#include <86box/mem.h>
#include <86box/pci.h>
#include <86box/rom.h>
#include <86box/video.h>
#include <86box/nv/vid_nv.h>
#include <86box/nv/vid_nv3.h>
#include <86box/nv/vid_nv3_formats.h>  // Include new formats header

// Define Windows 9x/2000/ME specific transparency colors
#define WIN9X_TRANSPARENT_COLOR_16BIT 0xF81F  // Magenta in RGB565
#define WIN9X_TRANSPARENT_COLOR_15BIT 0x7C1F  // Magenta in RGB555

// Standard transparency color for 8bpp palette mode
// Must be initialized at runtime since this might be configurable
static uint32_t nv3_transparency_color_8bpp = 0xFF00FF;  // Default to magenta

// Initialize transparency handling
void nv3_init_transparency(void) {
    // If transparency color wasn't set already in pgraph, set a default
    if (nv3->pgraph.transparency_color == 0) {
        nv3->pgraph.transparency_color = nv3_transparency_color_8bpp;
    }
}

// Check if a pixel should be treated as transparent in Windows ME/2000
bool nv3_is_win9x_transparent_pixel(uint32_t color, int format) {
    switch (format) {
        case 4:  // NV3_CONTEXT_SURFACE_FORMAT_A8R8G8B8
            // For 32-bit ARGB format - check alpha channel
            return ((color & 0xFF000000) == 0);
            
        case 3:  // NV3_CONTEXT_SURFACE_FORMAT_R5G6B5
            // For 16-bit RGB565 format (magenta is typically used as transparency)
            return ((color & 0xFFFF) == WIN9X_TRANSPARENT_COLOR_16BIT);
            
        case 2:  // NV3_CONTEXT_SURFACE_FORMAT_X1R5G5B5
            // For 15-bit RGB555 format
            return ((color & 0x7FFF) == WIN9X_TRANSPARENT_COLOR_15BIT);
            
        case 1:  // NV3_CONTEXT_SURFACE_FORMAT_Y8
            // For 8-bit palette format
            return (color == nv3->pgraph.transparency_color);
            
        default:
            return false;
    }
}

// Enhanced pixel writing function that handles Windows ME/2000 taskbar icons
void nv3_render_write_pixel_win9x(nv3_coord_16_t position, uint32_t color, nv3_grobj_t grobj)
{
    // Skip rendering if position is outside the viewport
    if (position.x >= nv3->nvbase.svga.hdisp || position.y >= nv3->nvbase.svga.dispend || 
        position.x < 0 || position.y < 0)
        return;

    // Detect if we're in Windows 9x/ME/2000 mode
    bool is_win9x_mode = ((nv3->nvbase.svga.crtc[NV3_CRTC_REGISTER_VRETRACESTART] >> 1) & 0x01) == 0;
    
    // Check if this is in the taskbar region (bottom 30-32 pixels of screen)
    bool is_taskbar_region = (position.y >= (nv3->nvbase.svga.dispend - 32));
    
    // Get surface format from grobj
    int format = (grobj.grobj_0 & 0x7);
    
    // If in Windows 9x mode and in taskbar region, check for transparency
    if (is_win9x_mode && is_taskbar_region) {
        // Don't render transparent pixels in the taskbar
        if (nv3_is_win9x_transparent_pixel(color, format)) {
            return;
        }
        
        // For non-transparent pixels in the taskbar, ensure proper color rendering
        if (nv3->nvbase.svga.bpp == 16 || nv3->nvbase.svga.bpp == 15) {
            // Special handling for 16/15 bit color in taskbar (ensure proper expansion)
            uint32_t expanded_color = color;
            
            // Apply proper color expansion for 16/15-bit colors if needed
            if (format != 4) {  // Not 32-bit ARGB
                expanded_color = nv3->nvbase.svga.conv_16to32(&nv3->nvbase.svga, color & 0xFFFF, 
                    (nv3->nvbase.svga.bpp == 16) ? 16 : 15);
            }
            
            // Write to framebuffer
            uint32_t vram_addr = nv3_render_get_vram_address(position, grobj);
            if (vram_addr != 0xFFFFFFFF) {
                // Write expanded color to VRAM
                if (nv3->nvbase.svga.bpp == 16) {
                    *(uint16_t*)&nv3->nvbase.svga.vram[vram_addr] = expanded_color & 0xFFFF;
                } else {
                    *(uint32_t*)&nv3->nvbase.svga.vram[vram_addr] = expanded_color;
                }
                
                // Update screen
                nv3->nvbase.svga.fullchange = 1;
                uint32_t* p = &nv3->nvbase.svga.monitor->target_buffer->line[position.y][position.x];
                *p = expanded_color;
            }
            return;
        }
    }
    
    // Use standard pixel writing for non-taskbar regions or non-Windows 9x mode
    nv3_render_write_pixel(position, color, grobj);
}