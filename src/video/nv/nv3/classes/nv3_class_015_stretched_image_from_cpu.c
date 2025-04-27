/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3: Methods for class 0x15 (stretched image from cpu to memory)
 *
 *
 * Authors: Connor Hyde, <mario64crashed@gmail.com>
 *          frostbite2000
 *
 *          Copyright 2024-2025 Connor Hyde
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

// State tracking for stretched image operations
typedef struct {
    nv3_coord_16_t dest_point;      // Destination start position
    nv3_coord_16_t dest_size;       // Destination size (stretched)
    nv3_coord_16_t source_size;     // Source size
    nv3_coord_16_t current_src;     // Current position in source image
    nv3_coord_16_t current_dest;    // Current position in destination
    uint32_t format;                // Image format
    bool initialized;               // Whether parameters have been set
} nv3_stretch_image_state_t;

static nv3_stretch_image_state_t stretch_state = {0};

// Reset the stretching state
static void nv3_stretch_reset(void) {
    memset(&stretch_state, 0, sizeof(nv3_stretch_image_state_t));
}

// Handle drawing of a stretched pixel
static void nv3_stretch_draw_pixel(uint32_t color, nv3_grobj_t grobj) {
    // Calculate scaling ratios
    float x_ratio = (float)stretch_state.dest_size.x / stretch_state.source_size.x;
    float y_ratio = (float)stretch_state.dest_size.y / stretch_state.source_size.y;
    
    // Calculate destination position based on current source position
    nv3_coord_16_t dest_pos = {
        .x = stretch_state.dest_point.x + (int)(stretch_state.current_src.x * x_ratio),
        .y = stretch_state.dest_point.y + (int)(stretch_state.current_src.y * y_ratio)
    };
    
    // Ensure we're within destination bounds
    if (dest_pos.x < (stretch_state.dest_point.x + stretch_state.dest_size.x) &&
        dest_pos.y < (stretch_state.dest_point.y + stretch_state.dest_size.y)) {
        // Use the enhanced pixel writing function that handles Windows ME/2000 UI elements
        nv3_render_write_pixel_win9x(dest_pos, color, grobj);
    }
    
    // Update source position
    stretch_state.current_src.x++;
    if (stretch_state.current_src.x >= stretch_state.source_size.x) {
        stretch_state.current_src.x = 0;
        stretch_state.current_src.y++;
        
        // Reset when entire image has been processed
        if (stretch_state.current_src.y >= stretch_state.source_size.y) {
            stretch_state.current_src.y = 0;
        }
    }
}

// Handle method calls for the Stretched Image from CPU class (0x15)
void nv3_class_015_method(uint32_t param, uint32_t method_id, nv3_ramin_context_t context, nv3_grobj_t grobj) {
    nv_log_verbose_only("Stretched Image Method: 0x%04x Param: 0x%08x\n", method_id, param);
    
    switch (method_id) {
        // Set destination position
        case 0x300:
            stretch_state.dest_point.x = param & 0xFFFF;
            stretch_state.dest_point.y = (param >> 16) & 0xFFFF;
            stretch_state.current_dest = stretch_state.dest_point;
            nv_log("Method Execution: Stretched Image Dest=%d,%d\n", 
                   stretch_state.dest_point.x, stretch_state.dest_point.y);
            break;
            
        // Set destination size (stretched size)
        case 0x304:
            stretch_state.dest_size.x = param & 0xFFFF;
            stretch_state.dest_size.y = (param >> 16) & 0xFFFF;
            nv_log("Method Execution: Stretched Image Size=%d,%d\n", 
                   stretch_state.dest_size.x, stretch_state.dest_size.y);
            break;
            
        // Set source size
        case 0x308:
            stretch_state.source_size.x = param & 0xFFFF;
            stretch_state.source_size.y = (param >> 16) & 0xFFFF;
            stretch_state.current_src.x = 0;
            stretch_state.current_src.y = 0;
            stretch_state.initialized = (stretch_state.source_size.x > 0 && 
                                        stretch_state.source_size.y > 0 &&
                                        stretch_state.dest_size.x > 0 && 
                                        stretch_state.dest_size.y > 0);
            nv_log("Method Execution: Stretched Image SRC Size=%d,%d\n", 
                   stretch_state.source_size.x, stretch_state.source_size.y);
            break;
            
        // Set format
        case 0x30C:
            stretch_state.format = param;
            nv_log("Method Execution: Stretched Image Format=0x%08x\n", stretch_state.format);
            break;
            
        default:
            // Color data methods (0x400+)
            if (method_id >= 0x400 && method_id < 0x500) {
                if (stretch_state.initialized) {
                    uint32_t color_format = grobj.grobj_0 & 0x7;
                    uint32_t bpp = nv3->nvbase.svga.bpp;
                    
                    switch (bpp) {
                        case 8:
                            // Process 4 pixels (8-bit each)
                            for (int i = 0; i < 4; i++) {
                                uint8_t pixel = (param >> (i * 8)) & 0xFF;
                                nv3_stretch_draw_pixel(pixel, grobj);
                            }
                            break;
                            
                        case 15:
                        case 16:
                            // Process 2 pixels (16-bit each)
                            for (int i = 0; i < 2; i++) {
                                uint16_t pixel = (param >> (i * 16)) & 0xFFFF;
                                nv3_stretch_draw_pixel(pixel, grobj);
                            }
                            break;
                            
                        case 32:
                            // Process 1 pixel (32-bit)
                            nv3_stretch_draw_pixel(param, grobj);
                            break;
                    }
                } else {
                    nv_log("Warning: Stretched Image color data received before parameters set\n");
                }
            } else {
                warning("%s: Invalid or unimplemented method 0x%04x\n", 
                       nv3_class_names[context.class_id & 0x1F], method_id);
                nv3_pgraph_interrupt_invalid(NV3_PGRAPH_INTR_1_SOFTWARE_METHOD_PENDING);
            }
            return;
    }
}