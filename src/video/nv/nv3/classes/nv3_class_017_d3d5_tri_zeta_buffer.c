/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3 Class 0x17: Direct3D 5 accelerated triangle with Z-buffer
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
#include <math.h>
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

// D3D5 Triangle state variables
typedef struct nv3_d3d5_state {
    uint32_t texture_offset;                   // Current texture memory offset
    nv3_d3d5_texture_format_t texture_format;  // Current texture format
    nv3_d3d5_texture_filter_t texture_filter;  // Current texture filtering settings
    nv3_color_expanded_t fog_color;            // Current fog color
    nv3_d3d5_control_out_t control_out;        // Current output control settings
    nv3_d3d5_alpha_control_t alpha_control;    // Current alpha control settings
    uint32_t vertex_count;                     // Number of vertices received
    nv3_d3d5_coordinate_t vertex_buffer[3];    // Vertex buffer for triangle
} nv3_d3d5_state_t;

// Current D3D5 rendering state
static nv3_d3d5_state_t d3d5_state;

// Forward declarations
static void nv3_d3d5_render_triangle(nv3_grobj_t grobj);
static uint32_t sample_texture(float u, float v, nv3_grobj_t grobj);
static uint32_t calculate_depth(float depth);
static uint32_t perform_z_buffer_test(uint32_t x, uint32_t y, uint32_t z, nv3_grobj_t grobj);

// Reset the D3D5 state
static void nv3_d3d5_reset_state(void) {
    memset(&d3d5_state, 0, sizeof(nv3_d3d5_state_t));
}

// Handle method calls for D3D5 accelerated triangle (class 0x17)
void nv3_class_017_method(uint32_t param, uint32_t method_id, nv3_ramin_context_t context, nv3_grobj_t grobj) {
    if (!nv3) return;
    
    nv_log_verbose_only("D3D5 Triangle Method: 0x%04x Param: 0x%08x\n", method_id, param);
    
    // Determine which method was called
    switch (method_id) {
        // Set texture offset in VRAM
        case 0x0300:
            d3d5_state.texture_offset = param & nv3->nvbase.svga.vram_mask;
            nv_log_verbose_only("D3D5: Set texture offset to 0x%08x\n", d3d5_state.texture_offset);
            break;
            
        // Set texture format
        case 0x0304:
            memcpy(&d3d5_state.texture_format, &param, sizeof(nv3_d3d5_texture_format_t));
            nv_log_verbose_only("D3D5: Set texture format: color_key=%d, format=%d, min=%d, max=%d\n", 
                d3d5_state.texture_format.color_key_enabled, 
                d3d5_state.texture_format.color_format,
                d3d5_state.texture_format.size_min,
                d3d5_state.texture_format.size_max);
            break;
            
        // Set texture filter parameters
        case 0x0308:
            memcpy(&d3d5_state.texture_filter, &param, sizeof(nv3_d3d5_texture_filter_t));
            nv_log_verbose_only("D3D5: Set texture filtering: spread_x=%d, spread_y=%d, mipmap=%d, turbo=%d\n",
                d3d5_state.texture_filter.spread_x,
                d3d5_state.texture_filter.spread_y,
                d3d5_state.texture_filter.mipmap,
                d3d5_state.texture_filter.turbo);
            break;
            
        // Set fog color
        case 0x030C:
            d3d5_state.fog_color = nv3_render_expand_color(param, grobj);
            nv_log_verbose_only("D3D5: Set fog color to 0x%08x\n", param);
            break;
            
        // Set control output parameters
        case 0x0310:
            memcpy(&d3d5_state.control_out, &param, sizeof(nv3_d3d5_control_out_t));
            nv_log_verbose_only("D3D5: Set control output parameters\n");
            break;
            
        // Set alpha control parameters
        case 0x0314:
            memcpy(&d3d5_state.alpha_control, &param, sizeof(nv3_d3d5_alpha_control_t));
            nv_log_verbose_only("D3D5: Set alpha control parameters\n");
            break;
            
        // Process vertex data
        default:
            if (method_id >= 0x0400 && method_id < 0x0580) {
                int vertex_offset = ((method_id - 0x0400) / 0x20);
                int param_type = (method_id - 0x0400) % 0x20;
                
                if (vertex_offset < 3) { // Only handle the first 3 vertices for a triangle
                    // Handle different vertex parameters
                    switch (param_type) {
                        // Specular reflection
                        case 0x00:
                            memcpy(&d3d5_state.vertex_buffer[vertex_offset].specular_reflection_parameters, &param, sizeof(nv3_d3d5_specular_t));
                            break;
                            
                        // Vertex color
                        case 0x04:
                            d3d5_state.vertex_buffer[vertex_offset].color = nv3_render_expand_color(param, grobj);
                            break;
                            
                        // X coordinate
                        case 0x08:
                            d3d5_state.vertex_buffer[vertex_offset].x = *(float*)&param;
                            break;
                            
                        // Y coordinate
                        case 0x0C:
                            d3d5_state.vertex_buffer[vertex_offset].y = *(float*)&param;
                            break;
                            
                        // Z coordinate (depth)
                        case 0x10:
                            d3d5_state.vertex_buffer[vertex_offset].z = *(float*)&param;
                            break;
                            
                        // Reciprocal W (for perspective correction)
                        case 0x14:
                            d3d5_state.vertex_buffer[vertex_offset].m = *(float*)&param;
                            break;
                            
                        // U texture coordinate
                        case 0x18:
                            d3d5_state.vertex_buffer[vertex_offset].u = *(float*)&param;
                            break;
                            
                        // V texture coordinate
                        case 0x1C:
                            d3d5_state.vertex_buffer[vertex_offset].v = *(float*)&param;
                            
                            // Check if we've completed the last vertex of the triangle
                            if (vertex_offset == 2) {
                                // We now have a complete triangle, render it
                                nv3_d3d5_render_triangle(grobj);
                                d3d5_state.vertex_count = 0;
                            } else {
                                d3d5_state.vertex_count++;
                            }
                            break;
                    }
                }
            } else {
                nv_log("D3D5: Unknown method ID: 0x%04x\n", method_id);
            }
            break;
    }
}

// Sample a texture at the given UV coordinates
static uint32_t sample_texture(float u, float v, nv3_grobj_t grobj) {
    // Apply texture wrap mode
    switch (d3d5_state.control_out.wrap_u) {
        case nv3_d3d5_texture_wrap_mode_cylindrical:
            // Implement cylindrical wrapping (similar to wrap but with different edge handling)
            u = fmodf(u, 1.0f);
            if (u < 0) u += 1.0f;
            break;
            
        case nv3_d3d5_texture_wrap_mode_wrap:
            u = fmodf(u, 1.0f);
            if (u < 0) u += 1.0f;
            break;
            
        case nv3_d3d5_texture_wrap_mode_mirror:
            u = fmodf(u, 2.0f);
            if (u > 1.0f) u = 2.0f - u;
            break;
            
        case nv3_d3d5_texture_wrap_mode_clamp:
            if (u < 0.0f) u = 0.0f;
            if (u > 1.0f) u = 1.0f;
            break;
    }
    
    switch (d3d5_state.control_out.wrap_v) {
        case nv3_d3d5_texture_wrap_mode_cylindrical:
            // Implement cylindrical wrapping (similar to wrap but with different edge handling)
            v = fmodf(v, 1.0f);
            if (v < 0) v += 1.0f;
            break;
            
        case nv3_d3d5_texture_wrap_mode_wrap:
            v = fmodf(v, 1.0f);
            if (v < 0) v += 1.0f;
            break;
            
        case nv3_d3d5_texture_wrap_mode_mirror:
            v = fmodf(v, 2.0f);
            if (v > 1.0f) v = 2.0f - v;
            break;
            
        case nv3_d3d5_texture_wrap_mode_clamp:
            if (v < 0.0f) v = 0.0f;
            if (v > 1.0f) v = 1.0f;
            break;
    }
    
    // Determine texture size and format
    uint32_t texture_size = 1 << d3d5_state.texture_format.size_max;
    uint32_t x = (uint32_t)(u * texture_size) & (texture_size - 1);
    uint32_t y = (uint32_t)(v * texture_size) & (texture_size - 1);
    
    // Calculate texture pixel offset
    uint32_t texel_offset = d3d5_state.texture_offset;
    
    // Add pixel offset based on format
    switch (d3d5_state.texture_format.color_format) {
        case nv3_d3d5_pixel_format_le_a1r5g5b5:
        case nv3_d3d5_pixel_format_le_x1r5g5b5:
        case nv3_d3d5_pixel_format_le_a4r4g4b4:
        case nv3_d3d5_pixel_format_le_r5g6b5:
            texel_offset += (y * texture_size + x) * 2;  // 16bpp formats
            
            // Read 16-bit color value
            uint16_t texel = *(uint16_t*)&nv3->nvbase.svga.vram[texel_offset & nv3->nvbase.svga.vram_mask];
            
            // Convert texel to 32-bit ARGB based on format
            uint32_t r, g, b, a;
            switch (d3d5_state.texture_format.color_format) {
                case nv3_d3d5_pixel_format_le_a1r5g5b5:
                    a = (texel & 0x8000) ? 0xFF : 0;
                    r = ((texel >> 10) & 0x1F) << 3;
                    g = ((texel >> 5) & 0x1F) << 3;
                    b = (texel & 0x1F) << 3;
                    break;
                    
                case nv3_d3d5_pixel_format_le_x1r5g5b5:
                    a = 0xFF;
                    r = ((texel >> 10) & 0x1F) << 3;
                    g = ((texel >> 5) & 0x1F) << 3;
                    b = (texel & 0x1F) << 3;
                    break;
                    
                case nv3_d3d5_pixel_format_le_a4r4g4b4:
                    a = ((texel >> 12) & 0xF) << 4;
                    r = ((texel >> 8) & 0xF) << 4;
                    g = ((texel >> 4) & 0xF) << 4;
                    b = (texel & 0xF) << 4;
                    break;
                    
                case nv3_d3d5_pixel_format_le_r5g6b5:
                    a = 0xFF;
                    r = ((texel >> 11) & 0x1F) << 3;
                    g = ((texel >> 5) & 0x3F) << 2;
                    b = (texel & 0x1F) << 3;
                    break;
                    
                default:
                    a = r = g = b = 0xFF;
            }
            
            return (a << 24) | (r << 16) | (g << 8) | b;
            
        default:
            // Unimplemented formats return white
            return 0xFFFFFFFF;
    }
}

// Calculate depth value for Z-buffer testing
static uint32_t calculate_depth(float depth) {
    // RIVA 128 uses a 16-bit Z-buffer
    // Map float range [0.0, 1.0] to [0, 65535]
    if (depth < 0.0f) depth = 0.0f;
    if (depth > 1.0f) depth = 1.0f;
    
    return (uint32_t)(depth * 65535.0f);
}

// Perform Z-buffer test at the given screen coordinate
static uint32_t perform_z_buffer_test(uint32_t x, uint32_t y, uint32_t z, nv3_grobj_t grobj) {
    // Calculate Z-buffer address
    // The Z-buffer is typically a separate surface in VRAM
    // For simplicity, we'll assume it's stored after the color buffer
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
    switch (d3d5_state.control_out.zeta_buffer_compare) {
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
    
    // If test passes and we need to write Z, update Z-buffer
    if (pass_test && (d3d5_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_zeta ||
                      d3d5_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_alpha_zeta ||
                      d3d5_state.control_out.zeta_write == nv3_d3d5_buffer_write_control_always)) {
        // Write new Z value
        *(uint16_t*)&nv3->nvbase.svga.vram[zbuffer_address] = (uint16_t)z;
    }
    
    return pass_test;
}

// Render a 3D triangle with perspective-correct texturing
static void nv3_d3d5_render_triangle(nv3_grobj_t grobj) {
    nv_log_verbose_only("D3D5: Rendering triangle\n");
    
    // Get vertex coordinates
    float x1 = d3d5_state.vertex_buffer[0].x;
    float y1 = d3d5_state.vertex_buffer[0].y;
    float z1 = d3d5_state.vertex_buffer[0].z;
    float x2 = d3d5_state.vertex_buffer[1].x;
    float y2 = d3d5_state.vertex_buffer[1].y;
    float z2 = d3d5_state.vertex_buffer[1].z;
    float x3 = d3d5_state.vertex_buffer[2].x;
    float y3 = d3d5_state.vertex_buffer[2].y;
    float z3 = d3d5_state.vertex_buffer[2].z;
    
    // Perspective correction factors (1/w)
    float w1 = d3d5_state.vertex_buffer[0].m;
    float w2 = d3d5_state.vertex_buffer[1].m;
    float w3 = d3d5_state.vertex_buffer[2].m;
    
    // Texture coordinates
    float u1 = d3d5_state.vertex_buffer[0].u;
    float v1 = d3d5_state.vertex_buffer[0].v;
    float u2 = d3d5_state.vertex_buffer[1].u;
    float v2 = d3d5_state.vertex_buffer[1].v;
    float u3 = d3d5_state.vertex_buffer[2].u;
    float v3 = d3d5_state.vertex_buffer[2].v;
    
    // Vertex colors
    nv3_color_expanded_t c1 = d3d5_state.vertex_buffer[0].color;
    nv3_color_expanded_t c2 = d3d5_state.vertex_buffer[1].color;
    nv3_color_expanded_t c3 = d3d5_state.vertex_buffer[2].color;
    
    // Compute bounding box of the triangle
    int min_x = (int)floorf(fminf(fminf(x1, x2), x3));
    int min_y = (int)floorf(fminf(fminf(y1, y2), y3));
    int max_x = (int)ceilf(fmaxf(fmaxf(x1, x2), x3));
    int max_y = (int)ceilf(fmaxf(fmaxf(y1, y2), y3));
    
    // Clip against screen boundaries
    min_x = MAX(min_x, 0);
    min_y = MAX(min_y, 0);
    max_x = MIN(max_x, nv3->nvbase.svga.hdisp - 1);
    max_y = MIN(max_y, nv3->nvbase.svga.vdisp - 1);
    
    // Compute triangle area
    float area = (x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1);
    
    // Backface culling
    if (d3d5_state.control_out.culling_algorithm != nv3_d3d5_culling_algorithm_none) {
        bool is_clockwise = (area > 0);
        
        if ((d3d5_state.control_out.culling_algorithm == nv3_d3d5_culling_algorithm_clockwise && is_clockwise) ||
            (d3d5_state.control_out.culling_algorithm == nv3_d3d5_culling_algorithm_counterclockwise && !is_clockwise)) {
            return;  // Cull this triangle
        }
    }
    
    // Skip rendering if area is zero (degenerate triangle)
    if (area == 0) return;
    
    // Reciprocal of area for barycentric coordinate calculation
    float inv_area = 1.0f / area;
    
    // Rasterize triangle
    for (int y = min_y; y <= max_y; y++) {
        for (int x = min_x; x <= max_x; x++) {
            // Compute barycentric coordinates
            float px = x + 0.5f;  // Center of pixel
            float py = y + 0.5f;
            
            // Compute edge functions for barycentric coordinates
            float e12 = (px - x1) * (y2 - y1) - (py - y1) * (x2 - x1);
            float e23 = (px - x2) * (y3 - y2) - (py - y2) * (x3 - x2);
            float e31 = (px - x3) * (y1 - y3) - (py - y3) * (x1 - x3);
            
            // Compute barycentric coordinates
            float w1_px = e23 * inv_area;
            float w2_px = e31 * inv_area;
            float w3_px = 1.0f - w1_px - w2_px;
            
            // Check if point is inside triangle
            if (w1_px >= 0 && w2_px >= 0 && w3_px >= 0) {
                // Perspective-correct interpolation of attributes
                float z_interpolated = w1_px * z1 + w2_px * z2 + w3_px * z3;
                uint32_t depth = calculate_depth(z_interpolated);
                
                // Z-buffer test
                if (perform_z_buffer_test(x, y, depth, grobj)) {
                    // Perspective-correct texture coordinates
                    float w_interpolated = 1.0f / (w1_px * w1 + w2_px * w2 + w3_px * w3);
                    
                    float u_over_w = w1_px * (u1 * w1) + w2_px * (u2 * w2) + w3_px * (u3 * w3);
                    float v_over_w = w1_px * (v1 * w1) + w2_px * (v2 * w2) + w3_px * (v3 * w3);
                    
                    float u_interpolated = u_over_w * w_interpolated;
                    float v_interpolated = v_over_w * w_interpolated;
                    
                    // Fetch texel
                    uint32_t texel = sample_texture(u_interpolated, v_interpolated, grobj);
                    
                    // Interpolate vertex colors
                    float r_interp = w1_px * c1.r + w2_px * c2.r + w3_px * c3.r;
                    float g_interp = w1_px * c1.g + w2_px * c2.g + w3_px * c3.g;
                    float b_interp = w1_px * c1.b + w2_px * c2.b + w3_px * c3.b;
                    float a_interp = w1_px * c1.a + w2_px * c2.a + w3_px * c3.a;
                    
                    // Convert interpolated colors to 8-bit components
                    uint32_t r = (uint32_t)(r_interp / 4.0f);  // Convert from 10-bit to 8-bit
                    uint32_t g = (uint32_t)(g_interp / 4.0f);
                    uint32_t b = (uint32_t)(b_interp / 4.0f);
                    uint32_t a = (uint32_t)(a_interp);
                    
                    // Clamp color values
                    r = MIN(r, 255);
                    g = MIN(g, 255);
                    b = MIN(b, 255);
                    a = MIN(a, 255);
                    
                    // Get texel colors
                    uint32_t texel_a = (texel >> 24) & 0xFF;
                    uint32_t texel_r = (texel >> 16) & 0xFF;
                    uint32_t texel_g = (texel >> 8) & 0xFF;
                    uint32_t texel_b = texel & 0xFF;
                    
                    // Final pixel color depends on rendering mode
                    uint32_t final_r, final_g, final_b, final_a;
                    
                    // Simplified blend mode (actual hardware has more complex modes)
                    final_r = (r * texel_r) >> 8;
                    final_g = (g * texel_g) >> 8;
                    final_b = (b * texel_b) >> 8;
                    final_a = (a * texel_a) >> 8;
                    
                    // Construct final color
                    uint32_t final_color = (final_a << 24) | (final_r << 16) | (final_g << 8) | final_b;
                    
                    // Write pixel to framebuffer
                    nv3_position_16_t pos = { x, y };
                    nv3_render_write_pixel(pos, final_color, grobj);
                }
            }
        }
    }
}

// Initialize D3D5 triangle rendering state
void nv3_class_017_init(void) {
    nv3_d3d5_reset_state();
    nv_log("D3D5 Triangle class initialized\n");
}