/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3 Class 0x18: Point with Z-buffer
 *          Implementation for the RIVA 128 / RIVA 128 ZX 3D core
 *
 * Authors: Connor Hyde <mario64crashed@gmail.com>
 *
 *          Copyright 2024-2025 Connor Hyde
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
#include <86box/nv/classes/vid_nv3_classes.h>
#include <86box/nv/render/vid_nv3_render.h>

// State for the Point Z-buffer class
typedef struct nv3_point_zeta_state {
    nv3_d3d5_control_out_t control_out;       // Output control settings
    nv3_d3d5_alpha_control_t alpha_control;   // Alpha control settings
    nv3_position_16_t point;                  // Current point position
    nv3_zeta_buffer_t zeta_buffer[8];         // Z-buffer data for points
    int buffer_index;                         // Current buffer index
} nv3_point_zeta_state_t;

// Current rendering state
static nv3_point_zeta_state_t point_zeta_state;

// Forward declarations
static void nv3_point_zeta_render(nv3_grobj_t grobj);

// Reset point zeta buffer state
static void nv3_point_zeta_reset_state(void) {
    memset(&point_zeta_state, 0, sizeof(nv3_point_zeta_state_t));
}

// Handle method calls for Point with Z-buffer (class 0x18)
void nv3_class_018_method(uint32_t param, uint32_t method_id, nv3_ramin_context_t context, nv3_grobj_t grobj) {
    if (!nv3) return;
    
    nv_log_verbose_only("Point Z-Buffer Method: 0x%04x Param: 0x%08x\n", method_id, param);
    
    // Determine which method was called
    switch (method_id) {
        // Set control output parameters
        case 0x0300:
            memcpy(&point_zeta_state.control_out, &param, sizeof(nv3_d3d5_control_out_t));
            nv_log_verbose_only("Point Z-Buffer: Set control output parameters\n");
            break;
            
        // Set alpha control parameters
        case 0x0304:
            memcpy(&point_zeta_state.alpha_control, &param, sizeof(nv3_d3d5_alpha_control_t));
            nv_log_verbose_only("Point Z-Buffer: Set alpha control parameters\n");
            break;
            
        // Set point position
        case 0x0308:
            memcpy(&point_zeta_state.point, &param, sizeof(nv3_position_16_t));
            nv_log_verbose_only("Point Z-Buffer: Set point position to (%d, %d)\n", 
                point_zeta_state.point.x, point_zeta_state.point.y);
            break;
            
        // Process Z-buffer data
        default:
            if (method_id >= 0x0310 && method_id < 0x0350) {
                int buffer_offset = (method_id - 0x0310) / 8;
                int param_type = (method_id - 0x0310) % 8;
                
                if (buffer_offset < 8) {
                    // Handle different Z-buffer parameters
                    switch (param_type) {
                        // Z-buffer color
                        case 0:
                            point_zeta_state.zeta_buffer[buffer_offset].color = nv3_render_expand_color(param, grobj);
                            break;
                            
                        // Z-buffer depth value
                        case 4:
                            point_zeta_state.zeta_buffer[buffer_offset].zeta = param;
                            
                            // If this is the last parameter for this buffer entry, render the point
                            if (buffer_offset >= point_zeta_state.buffer_index) {
                                point_zeta_state.buffer_index = buffer_offset + 1;
                                
                                // If we've received a complete set or this is the last expected entry, render
                                if (param_type == 4 && (buffer_offset == 7 || method_id + 8 > 0x0350)) {
                                    nv3_point_zeta_render(grobj);
                                    point_zeta_state.buffer_index = 0;
                                }
                            }
                            break;
                    }
                }
            } else {
                nv_log("Point Z-Buffer: Unknown method ID: 0x%04x\n", method_id);
            }
            break;
    }
}

// Render a point with Z-buffer testing
static void nv3_point_zeta_render(nv3_grobj_t grobj) {
    nv_log_verbose_only("Point Z-Buffer: Rendering %d points\n", point_zeta_state.buffer_index);
    
    // Get point position
    int x = point_zeta_state.point.x;
    int y = point_zeta_state.point.y;
    
    // Clip against screen boundaries
    if (x < 0 || y < 0 || x >= nv3->nvbase.svga.hdisp || y >= nv3->nvbase.svga.vdisp) {
        return;
    }
    
    // Process each buffered point
    for (int i = 0; i < point_zeta_state.buffer_index; i++) {
        // Get Z value and color for this point
        uint32_t z = point_zeta_state.zeta_buffer[i].zeta & 0xFFFF;  // 16-bit Z value
        nv3_color_expanded_t color = point_zeta_state.zeta_buffer[i].color;
        
        // Calculate Z-buffer address
        uint32_t zbuffer_offset = nv3->pgraph.boffset[1];  // Assuming buffer 1 is used for Z
        uint32_t zbuffer_pitch = nv3->pgraph.bpitch[1];
        
        if (zbuffer_pitch == 0) {
            // If no specific Z-buffer pitch is set, use screen width
            zbuffer_pitch = nv3->nvbase.svga.hdisp * 2;  // 2 bytes per Z value
        }
        
        uint32_t zbuffer_address = zbuffer_offset + y * zbuffer_pitch + x * 2;
        zbuffer_address &= nv3->nvbase.svga.vram_mask;
        
        // Read current Z value
        uint16_t current_z = *(uint16_t*)&nv3->nvbase.svga.vram[zbuffer_address];
        
        // Compare based on selected Z-buffer test
        bool pass_test = false;
        switch (point_zeta_state.control_out.zeta_buffer_compare) {
            case nv3_d3d5_buffer_comparison_always_false:
                pass_test = false;
                break;
                
            case nv3_d3d5_buffer_comparison_less_than:
                pass_test = (z < current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_equal:
                pass_test = (z == current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_less_or_equal:
                pass_test = (z <= current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_greater:
                pass_test = (z > current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_not_equal:
                pass_test = (z != current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_greater_or_equal:
                pass_test = (z >= current_z);
                break;
                
            case nv3_d3d5_buffer_comparison_always_true:
            default:
                pass_test = true;
                break;
        }
        
        // If the Z test passes, draw the point and update Z buffer if needed
        if (pass_test) {
            // If we need to write Z, update Z-buffer
            if (point_zeta_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_zeta ||
                point_zeta_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_alpha_zeta ||
                point_zeta_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_always) {
                // Write new Z value
                *(uint16_t*)&nv3->nvbase.svga.vram[zbuffer_address] = (uint16_t)z;
            }
            
            // Convert expanded color to 32-bit ARGB
            uint32_t r = (uint32_t)(color.r / 4.0f);  // Convert from 10-bit to 8-bit
            uint32_t g = (uint32_t)(color.g / 4.0f);
            uint32_t b = (uint32_t)(color.b / 4.0f);
            uint32_t a = (uint32_t)(color.a);
            
            // Clamp color values
            r = MIN(r, 255);
            g = MIN(g, 255);
            b = MIN(b, 255);
            a = MIN(a, 255);
            
            uint32_t argb = (a << 24) | (r << 16) | (g << 8) | b;
            
            // Write the point to the framebuffer
            nv3_position_16_t pos = { x, y };
            nv3_render_write_pixel(pos, argb, grobj);
        }
    }
}

// Initialize Point Z-buffer class
void nv3_class_018_init(void) {
    nv3_point_zeta_reset_state();
    nv_log("Point Z-Buffer class initialized\n");
}