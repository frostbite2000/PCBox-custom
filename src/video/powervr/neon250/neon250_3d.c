/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          VideoLogic PowerVR Neon 250 3D rendering core.
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
#include <math.h>
#include <stdbool.h>
#define HAVE_STDARG_H

#include "86box/86box.h"
#include "86box/device.h"
#include "86box/timer.h"
#include "86box/mem.h"
#include "86box/video.h"
#include "86box/vid_svga.h"
#include "86box/plat.h" /* For plat_timer_read() */
#include "86box/vid_svga_render.h" /* For svga_mark_dirty() */

#include "86box/vid_powervr_neon250.h"

/* PowerVR Neon 250 specific 3D registers */
#define NEON_3D_CONTROL       0x100  /* 3D Control Register */
#define NEON_3D_STATUS        0x101  /* 3D Status Register */
#define NEON_3D_ZBUFFER_ADDR  0x102  /* Z-Buffer Base Address */
#define NEON_3D_TEXTURE_ADDR  0x103  /* Texture Memory Base Address */
#define NEON_3D_DISPLAY_ADDR  0x104  /* Display Buffer Address */
#define NEON_3D_VERTEX_ADDR   0x105  /* Vertex Buffer Address */
#define NEON_3D_OBJECT_ADDR   0x106  /* Object List Address */
#define NEON_3D_SCISSOR_X     0x107  /* Scissor X Coordinates (left << 16 | right) */
#define NEON_3D_SCISSOR_Y     0x108  /* Scissor Y Coordinates (top << 16 | bottom) */
#define NEON_3D_FOG_COLOR     0x109  /* Fog Color (ARGB) */
#define NEON_3D_AMBIENT_COLOR 0x10A  /* Ambient Light Color (RGB) */
#define NEON_3D_VIEWPORT_X    0x10B  /* Viewport X Scale and Offset */
#define NEON_3D_VIEWPORT_Y    0x10C  /* Viewport Y Scale and Offset */
#define NEON_3D_VIEWPORT_Z    0x10D  /* Viewport Z Scale and Offset */
#define NEON_3D_CONFIG        0x10E  /* 3D Configuration */

/* PowerVR Neon 250 command list op codes */
#define NEON_OP_NOP           0x00   /* No operation */
#define NEON_OP_TRIANGLE      0x01   /* Draw triangle */
#define NEON_OP_TRIANGLESTRIP 0x02   /* Draw triangle strip */
#define NEON_OP_TRIANGLEFAN   0x03   /* Draw triangle fan */
#define NEON_OP_LINE          0x04   /* Draw line */
#define NEON_OP_LINESTRIP     0x05   /* Draw line strip */
#define NEON_OP_POINT         0x06   /* Draw point */
#define NEON_OP_TEXLOAD       0x07   /* Load texture */
#define NEON_OP_MATERIAL      0x08   /* Set material properties */
#define NEON_OP_MATRIX        0x09   /* Set transformation matrix */
#define NEON_OP_LIGHTPARAM    0x0A   /* Set light parameters */
#define NEON_OP_CLEAR         0x0B   /* Clear buffers */
#define NEON_OP_END           0xFF   /* End of command list */

/* Local prototypes */
void neon_3d_reset(neon250_t *neon250);
void neon_3d_process_commands(neon250_t *neon250);
static void neon_3d_draw_triangle(neon250_t *neon250, neon_vertex_t *v1, neon_vertex_t *v2, neon_vertex_t *v3);
static void neon_3d_draw_line(neon250_t *neon250, neon_vertex_t *v1, neon_vertex_t *v2);
static void neon_3d_draw_point(neon250_t *neon250, neon_vertex_t *v);
static void neon_3d_load_texture(neon250_t *neon250, uint32_t address, uint16_t width, uint16_t height, uint8_t format);
static void neon_3d_transform_vertex(neon250_t *neon250, neon_vertex_t *v);
static void neon_3d_apply_lighting(neon250_t *neon250, neon_vertex_t *v);
static void neon_3d_setup_matrices(neon250_t *neon250, uint32_t matrix_type, float *matrix);
static void neon_3d_configure_material(neon250_t *neon250, neon_material_t *material);
static void neon_3d_setup_light(neon250_t *neon250, uint32_t light_index, neon_light_t *light);
static void neon_3d_clear_buffers(neon250_t *neon250, bool clear_color, bool clear_z, uint32_t color, uint16_t z_value);

/* Initialize the 3D engine */
void neon_3d_init(neon250_t *neon250)
{
    /* Allocate memory for 3D state if not already allocated */
    if (!neon250->state_3d) {
        neon250->state_3d = (neon_3d_state_t *)malloc(sizeof(neon_3d_state_t));
        memset(neon250->state_3d, 0, sizeof(neon_3d_state_t));
    }
    
    /* Reset the 3D engine to default state */
    neon_3d_reset(neon250);
}

/* Reset the 3D engine */
void neon_3d_reset(neon250_t *neon250)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Clear all state variables */
    memset(state, 0, sizeof(neon_3d_state_t));
    
    /* Initialize default values */
    state->control = 0;
    state->status = 0;
    state->fog_color = 0x00808080;   /* Medium gray fog */
    state->ambient_color = 0x00202020; /* Dark gray ambient light */
    
    /* Set default viewport (full screen) */
    state->viewport_x = (0 << 16) | neon250->svga->hdisp;
    state->viewport_y = (0 << 16) | neon250->svga->vdisp;
    state->viewport_z = 0x00010000;   /* Z scale = 1.0, Z offset = 0.0 */
    
    /* Initialize scissor rect to full viewport */
    state->scissor_x = state->viewport_x;
    state->scissor_y = state->viewport_y;
    
    /* Initialize matrices to identity */
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state->matrices.world[i*4+j] = (i == j) ? 1.0f : 0.0f;
            state->matrices.view[i*4+j] = (i == j) ? 1.0f : 0.0f;
            state->matrices.projection[i*4+j] = (i == j) ? 1.0f : 0.0f;
            state->matrices.texture[i*4+j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    
    /* Set default material properties */
    state->current_material.diffuse = 0xFFFFFFFF;   /* White */
    state->current_material.specular = 0x00000000;  /* Black */
    state->current_material.ambient = 0xFFFFFFFF;   /* White */
    state->current_material.emissive = 0x00000000;  /* Black */
    state->current_material.power = 0.0f;
    state->current_material.shading_mode = 0;       /* Flat shading */
    state->current_material.blending_mode = 0;      /* No blending */
    state->current_material.texture_op = 0;         /* No texturing */
    state->current_material.texture_filter = 0;     /* Point filtering */
    state->current_material.z_write = true;
    state->current_material.z_test = true;
    
    /* Allocate Z-buffer if needed */
    if (neon250->svga) {
        uint32_t z_buffer_size = neon250->svga->hdisp * neon250->svga->vdisp * 2; /* 16-bit Z-buffer */
        
        if (state->z_buffer) {
            if (state->z_buffer_size < z_buffer_size) {
                free(state->z_buffer);
                state->z_buffer = NULL;
            }
        }
        
        if (!state->z_buffer) {
            state->z_buffer = (uint16_t *)malloc(z_buffer_size);
            state->z_buffer_size = z_buffer_size;
        }
        
        /* Clear Z-buffer to maximum depth */
        if (state->z_buffer) {
            memset(state->z_buffer, 0xFF, state->z_buffer_size);
        }
    }
    
    /* Reset rendering statistics */
    state->triangles_rendered = 0;
    state->lines_rendered = 0;
    state->points_rendered = 0;
    
    /* Clear state flags */
    state->render_in_progress = false;
    state->texture_enabled = false;
    state->fog_enabled = false;
    state->dithering_enabled = false;
    state->bilinear_filtering = false;
    state->wireframe_mode = false;
}

/* Close and clean up 3D engine resources */
void neon_3d_close(neon250_t *neon250)
{
    if (neon250->state_3d) {
        /* Free Z-buffer */
        if (neon250->state_3d->z_buffer) {
            free(neon250->state_3d->z_buffer);
            neon250->state_3d->z_buffer = NULL;
        }
        
        /* Free 3D state structure */
        free(neon250->state_3d);
        neon250->state_3d = NULL;
    }
}

/* Read from 3D registers */
uint32_t neon_3d_read(neon250_t *neon250, uint32_t addr)
{
    neon_3d_state_t *state = neon250->state_3d;
    uint32_t reg = addr >> 2;
    uint32_t value = 0;
    
    switch (reg) {
        case NEON_3D_CONTROL:
            value = state->control;
            break;
            
        case NEON_3D_STATUS:
            value = state->status;
            /* Set bit 0 if rendering is in progress */
            if (state->render_in_progress) {
                value |= 0x01;
            }
            break;
            
        case NEON_3D_ZBUFFER_ADDR:
            value = state->zbuffer_addr;
            break;
            
        case NEON_3D_TEXTURE_ADDR:
            value = state->texture_addr;
            break;
            
        case NEON_3D_DISPLAY_ADDR:
            value = state->display_addr;
            break;
            
        case NEON_3D_VERTEX_ADDR:
            value = state->vertex_addr;
            break;
            
        case NEON_3D_OBJECT_ADDR:
            value = state->object_addr;
            break;
            
        case NEON_3D_SCISSOR_X:
            value = state->scissor_x;
            break;
            
        case NEON_3D_SCISSOR_Y:
            value = state->scissor_y;
            break;
            
        case NEON_3D_FOG_COLOR:
            value = state->fog_color;
            break;
            
        case NEON_3D_AMBIENT_COLOR:
            value = state->ambient_color;
            break;
            
        case NEON_3D_VIEWPORT_X:
            value = state->viewport_x;
            break;
            
        case NEON_3D_VIEWPORT_Y:
            value = state->viewport_y;
            break;
            
        case NEON_3D_VIEWPORT_Z:
            value = state->viewport_z;
            break;
            
        case NEON_3D_CONFIG:
            value = state->config;
            break;
            
        default:
            value = 0xFFFFFFFF;
            break;
    }
    
    return value;
}

/* Write to 3D registers */
void neon_3d_write(neon250_t *neon250, uint32_t addr, uint32_t value)
{
    neon_3d_state_t *state = neon250->state_3d;
    uint32_t reg = addr >> 2;
    
    switch (reg) {
        case NEON_3D_CONTROL:
            state->control = value;
            /* Check if rendering should start (bit 0) */
            if (value & 0x01) {
                /* Start rendering */
                state->render_in_progress = true;
                state->render_start_time = plat_timer_read();
                
                /* Process command list */
                neon_3d_process_commands(neon250);
            }
            break;
            
        case NEON_3D_STATUS:
            /* Status is typically read-only, but some bits might be clearable */
            break;
            
        case NEON_3D_ZBUFFER_ADDR:
            state->zbuffer_addr = value & neon250->vram_mask;
            break;
            
        case NEON_3D_TEXTURE_ADDR:
            state->texture_addr = value & neon250->vram_mask;
            break;
            
        case NEON_3D_DISPLAY_ADDR:
            state->display_addr = value & neon250->vram_mask;
            break;
            
        case NEON_3D_VERTEX_ADDR:
            state->vertex_addr = value & neon250->vram_mask;
            break;
            
        case NEON_3D_OBJECT_ADDR:
            state->object_addr = value & neon250->vram_mask;
            break;
            
        case NEON_3D_SCISSOR_X:
            state->scissor_x = value;
            break;
            
        case NEON_3D_SCISSOR_Y:
            state->scissor_y = value;
            break;
            
        case NEON_3D_FOG_COLOR:
            state->fog_color = value;
            break;
            
        case NEON_3D_AMBIENT_COLOR:
            state->ambient_color = value;
            break;
            
        case NEON_3D_VIEWPORT_X:
            state->viewport_x = value;
            break;
            
        case NEON_3D_VIEWPORT_Y:
            state->viewport_y = value;
            break;
            
        case NEON_3D_VIEWPORT_Z:
            state->viewport_z = value;
            break;
            
        case NEON_3D_CONFIG:
            state->config = value;
            /* Update state flags based on config */
            state->texture_enabled = (value & 0x01) ? true : false;
            state->fog_enabled = (value & 0x02) ? true : false;
            state->dithering_enabled = (value & 0x04) ? true : false;
            state->bilinear_filtering = (value & 0x08) ? true : false;
            state->wireframe_mode = (value & 0x10) ? true : false;
            break;
    }
}

/* Process 3D rendering commands */
void neon_3d_process_commands(neon250_t *neon250)
{
    neon_3d_state_t *state = neon250->state_3d;
    uint32_t cmd_addr = state->object_addr;
    bool done = false;
    
    /* Ensure we have a valid command list address */
    if (!cmd_addr || (cmd_addr >= neon250->vram_size)) {
        state->render_in_progress = false;
        return;
    }
    
    /* Process commands until we reach the end or encounter an error */
    while (!done && cmd_addr < neon250->vram_size) {
        uint8_t op_code = neon250->vram[cmd_addr++];
        
        switch (op_code) {
            case NEON_OP_NOP:
                /* No operation, just skip */
                break;
                
            case NEON_OP_TRIANGLE:
                /* Draw a single triangle */
                {
                    uint32_t v_idx1, v_idx2, v_idx3;
                    neon_vertex_t v1, v2, v3;
                    
                    /* Read vertex indices */
                    v_idx1 = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    v_idx2 = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    v_idx3 = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    /* Load vertices */
                    memcpy(&v1, &neon250->vram[state->vertex_addr + v_idx1 * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                    memcpy(&v2, &neon250->vram[state->vertex_addr + v_idx2 * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                    memcpy(&v3, &neon250->vram[state->vertex_addr + v_idx3 * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                    
                    /* Transform vertices */
                    neon_3d_transform_vertex(neon250, &v1);
                    neon_3d_transform_vertex(neon250, &v2);
                    neon_3d_transform_vertex(neon250, &v3);
                    
                    /* Apply lighting if not using pre-lit vertices */
                    if (!(state->config & 0x20)) {
                        neon_3d_apply_lighting(neon250, &v1);
                        neon_3d_apply_lighting(neon250, &v2);
                        neon_3d_apply_lighting(neon250, &v3);
                    }
                    
                    /* Draw the triangle */
                    neon_3d_draw_triangle(neon250, &v1, &v2, &v3);
                    
                    /* Update statistics */
                    state->triangles_rendered++;
                }
                break;
                
            case NEON_OP_TRIANGLESTRIP:
                /* Draw a triangle strip */
                {
                    uint32_t num_vertices;
                    uint32_t *v_indices;
                    neon_vertex_t *vertices;
                    
                    /* Read number of vertices */
                    num_vertices = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    if (num_vertices < 3 || num_vertices > 1024) {
                        /* Invalid vertex count, abort */
                        done = true;
                        break;
                    }
                    
                    /* Allocate memory for vertex indices */
                    v_indices = (uint32_t*)malloc(num_vertices * sizeof(uint32_t));
                    
                    /* Read vertex indices */
                    for (uint32_t i = 0; i < num_vertices; i++) {
                        v_indices[i] = *((uint32_t*)&neon250->vram[cmd_addr]);
                        cmd_addr += 4;
                    }
                    
                    /* Allocate memory for vertices */
                    vertices = (neon_vertex_t*)malloc(num_vertices * sizeof(neon_vertex_t));
                    
                    /* Load and transform vertices */
                    for (uint32_t i = 0; i < num_vertices; i++) {
                        memcpy(&vertices[i], &neon250->vram[state->vertex_addr + v_indices[i] * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                        neon_3d_transform_vertex(neon250, &vertices[i]);
                        
                        /* Apply lighting if not using pre-lit vertices */
                        if (!(state->config & 0x20)) {
                            neon_3d_apply_lighting(neon250, &vertices[i]);
                        }
                    }
                    
                    /* Draw the triangles in the strip */
                    for (uint32_t i = 0; i < num_vertices - 2; i++) {
                        /* Alternate winding direction to maintain correct face orientation */
                        if (i % 2 == 0) {
                            neon_3d_draw_triangle(neon250, &vertices[i], &vertices[i+1], &vertices[i+2]);
                        } else {
                            neon_3d_draw_triangle(neon250, &vertices[i], &vertices[i+2], &vertices[i+1]);
                        }
                        
                        /* Update statistics */
                        state->triangles_rendered++;
                    }
                    
                    /* Free allocated memory */
                    free(v_indices);
                    free(vertices);
                }
                break;
                
            case NEON_OP_TRIANGLEFAN:
                /* Draw a triangle fan */
                {
                    uint32_t num_vertices;
                    uint32_t *v_indices;
                    neon_vertex_t *vertices;
                    
                    /* Read number of vertices */
                    num_vertices = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    if (num_vertices < 3 || num_vertices > 1024) {
                        /* Invalid vertex count, abort */
                        done = true;
                        break;
                    }
                    
                    /* Allocate memory for vertex indices */
                    v_indices = (uint32_t*)malloc(num_vertices * sizeof(uint32_t));
                    
                    /* Read vertex indices */
                    for (uint32_t i = 0; i < num_vertices; i++) {
                        v_indices[i] = *((uint32_t*)&neon250->vram[cmd_addr]);
                        cmd_addr += 4;
                    }
                    
                    /* Allocate memory for vertices */
                    vertices = (neon_vertex_t*)malloc(num_vertices * sizeof(neon_vertex_t));
                    
                    /* Load and transform vertices */
                    for (uint32_t i = 0; i < num_vertices; i++) {
                        memcpy(&vertices[i], &neon250->vram[state->vertex_addr + v_indices[i] * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                        neon_3d_transform_vertex(neon250, &vertices[i]);
                        
                        /* Apply lighting if not using pre-lit vertices */
                        if (!(state->config & 0x20)) {
                            neon_3d_apply_lighting(neon250, &vertices[i]);
                        }
                    }
                    
                    /* Draw the triangles in the fan */
                    for (uint32_t i = 1; i < num_vertices - 1; i++) {
                        neon_3d_draw_triangle(neon250, &vertices[0], &vertices[i], &vertices[i+1]);
                        
                        /* Update statistics */
                        state->triangles_rendered++;
                    }
                    
                    /* Free allocated memory */
                    free(v_indices);
                    free(vertices);
                }
                break;
                
            case NEON_OP_LINE:
                /* Draw a single line */
                {
                    uint32_t v_idx1, v_idx2;
                    neon_vertex_t v1, v2;
                    
                    /* Read vertex indices */
                    v_idx1 = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    v_idx2 = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    /* Load vertices */
                    memcpy(&v1, &neon250->vram[state->vertex_addr + v_idx1 * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                    memcpy(&v2, &neon250->vram[state->vertex_addr + v_idx2 * sizeof(neon_vertex_t)], sizeof(neon_vertex_t));
                    
                    /* Transform vertices */
                    neon_3d_transform_vertex(neon250, &v1);
                    neon_3d_transform_vertex(neon250, &v2);
                    
                    /* Apply lighting if not using pre-lit vertices */
                    if (!(state->config & 0x20)) {
                        neon_3d_apply_lighting(neon250, &v1);
                        neon_3d_apply_lighting(neon250, &v2);
                    }
                    
                    /* Draw the line */
                    neon_3d_draw_line(neon250, &v1, &v2);
                    
                    /* Update statistics */
                    state->lines_rendered++;
                }
                break;
                
            case NEON_OP_TEXLOAD:
                /* Load texture */
                {
                    uint32_t tex_addr;
                    uint16_t width, height;
                    uint8_t format;
                    
                    /* Read texture parameters */
                    tex_addr = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    width = *((uint16_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 2;
                    height = *((uint16_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 2;
                    format = neon250->vram[cmd_addr++];
                    
                    /* Load the texture */
                    neon_3d_load_texture(neon250, tex_addr, width, height, format);
                }
                break;
                
            case NEON_OP_MATERIAL:
                /* Set material properties */
                {
                    neon_material_t material;
                    
                    /* Read material properties */
                    memcpy(&material, &neon250->vram[cmd_addr], sizeof(neon_material_t));
                    cmd_addr += sizeof(neon_material_t);
                    
                    /* Configure material */
                    neon_3d_configure_material(neon250, &material);
                }
                break;
                
            case NEON_OP_MATRIX:
                /* Set transformation matrix */
                {
                    uint32_t matrix_type;
                    float matrix[16];
                    
                    /* Read matrix type */
                    matrix_type = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    /* Read matrix data (16 floats) */
                    memcpy(matrix, &neon250->vram[cmd_addr], 16 * sizeof(float));
                    cmd_addr += 16 * sizeof(float);
                    
                    /* Setup matrix */
                    neon_3d_setup_matrices(neon250, matrix_type, matrix);
                }
                break;
                
            case NEON_OP_LIGHTPARAM:
                /* Set light parameters */
                {
                    uint32_t light_index;
                    neon_light_t light;
                    
                    /* Read light index */
                    light_index = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    
                    /* Check if the light index is valid */
                    if (light_index >= 8) {
                        /* Invalid light index, abort */
                        done = true;
                        break;
                    }
                    
                    /* Read light parameters */
                    memcpy(&light, &neon250->vram[cmd_addr], sizeof(neon_light_t));
                    cmd_addr += sizeof(neon_light_t);
                    
                    /* Setup light */
                    neon_3d_setup_light(neon250, light_index, &light);
                }
                break;
                
            case NEON_OP_CLEAR:
                /* Clear buffers */
                {
                    uint32_t clear_flags;
                    uint32_t clear_color;
                    uint16_t clear_z;
                    
                    /* Read clear parameters */
                    clear_flags = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    clear_color = *((uint32_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 4;
                    clear_z = *((uint16_t*)&neon250->vram[cmd_addr]);
                    cmd_addr += 2;
                    
                    /* Clear buffers */
                    neon_3d_clear_buffers(neon250, 
                        (clear_flags & 0x01) != 0,  /* Clear color buffer */
                        (clear_flags & 0x02) != 0,  /* Clear Z buffer */
                        clear_color, clear_z);
                }
                break;
                
            case NEON_OP_END:
                /* End of command list */
                done = true;
                break;
                
            default:
                /* Unknown opcode, abort */
                done = true;
                break;
        }
    }
    
    /* Rendering is complete */
    state->render_in_progress = false;
    state->render_end_time = plat_timer_read();
    
    /* Set rendering complete bit in status register */
    state->status |= 0x02;
}

/* Transform a vertex using current matrices */
static void neon_3d_transform_vertex(neon250_t *neon250, neon_vertex_t *v)
{
    neon_3d_state_t *state = neon250->state_3d;
    float x, y, z, w;
    float *world = state->matrices.world;
    float *view = state->matrices.view;
    float *proj = state->matrices.projection;
    
    /* World transformation */
    x = v->x * world[0] + v->y * world[4] + v->z * world[8] + world[12];
    y = v->x * world[1] + v->y * world[5] + v->z * world[9] + world[13];
    z = v->x * world[2] + v->y * world[6] + v->z * world[10] + world[14];
    w = v->x * world[3] + v->y * world[7] + v->z * world[11] + world[15];
    
    /* View transformation */
    v->x = x * view[0] + y * view[4] + z * view[8] + w * view[12];
    v->y = x * view[1] + y * view[5] + z * view[9] + w * view[13];
    v->z = x * view[2] + y * view[6] + z * view[10] + w * view[14];
    w = x * view[3] + y * view[7] + z * view[11] + w * view[15];
    
    /* Projection transformation */
    x = v->x * proj[0] + v->y * proj[4] + v->z * proj[8] + w * proj[12];
    y = v->x * proj[1] + v->y * proj[5] + v->z * proj[9] + w * proj[13];
    z = v->x * proj[2] + v->y * proj[6] + v->z * proj[10] + w * proj[14];
    w = v->x * proj[3] + v->y * proj[7] + v->z * proj[11] + w * proj[15];
    
    /* Perspective divide */
    if (w != 0.0f) {
        v->x = x / w;
        v->y = y / w;
        v->z = z / w;
    }
    
    /* Transform normal if lighting is enabled */
    if (!(state->config & 0x20)) {
        float nx, ny, nz;
        
        /* Transform normal by world matrix (ignoring translation) */
        nx = v->nx * world[0] + v->ny * world[4] + v->nz * world[8];
        ny = v->nx * world[1] + v->ny * world[5] + v->nz * world[9];
        nz = v->nx * world[2] + v->ny * world[6] + v->nz * world[10];
        
        /* Normalize the transformed normal */
        float length = sqrtf(nx*nx + ny*ny + nz*nz);
        if (length > 0.0001f) {
            v->nx = nx / length;
            v->ny = ny / length;
            v->nz = nz / length;
        }
    }
    
    /* Transform texture coordinates if texture mapping is enabled */
    if (state->texture_enabled) {
        float tex_u = v->u;
        float tex_v = v->v;
        float *tex = state->matrices.texture;
        
        /* Apply texture transformation matrix */
        v->u = tex_u * tex[0] + tex_v * tex[4] + tex[12];
        v->v = tex_u * tex[1] + tex_v * tex[5] + tex[13];
    }
    
    /* Apply viewport transformation */
    uint32_t vp_x = state->viewport_x;
    uint32_t vp_y = state->viewport_y;
    uint32_t vp_z = state->viewport_z;
    
    int vp_width = vp_x & 0xFFFF;
    int vp_height = vp_y & 0xFFFF;
    int vp_x0 = (vp_x >> 16) & 0xFFFF;
    int vp_y0 = (vp_y >> 16) & 0xFFFF;
    float vp_scale_z = (float)((vp_z >> 16) & 0xFFFF) / 65536.0f;
    float vp_offset_z = (float)(vp_z & 0xFFFF) / 65536.0f;
    
    /* Convert normalized device coordinates to screen coordinates */
    v->x = vp_x0 + (v->x + 1.0f) * (vp_width / 2.0f);
    v->y = vp_y0 + (1.0f - v->y) * (vp_height / 2.0f);
    v->z = (v->z * vp_scale_z) + vp_offset_z;
}

/* Apply lighting to a vertex */
static void neon_3d_apply_lighting(neon250_t *neon250, neon_vertex_t *v)
{
    neon_3d_state_t *state = neon250->state_3d;
    float r = 0.0f, g = 0.0f, b = 0.0f;
    float ambient_r, ambient_g, ambient_b;
    
    /* Extract ambient light color */
    ambient_r = ((state->ambient_color >> 16) & 0xFF) / 255.0f;
    ambient_g = ((state->ambient_color >> 8) & 0xFF) / 255.0f;
    ambient_b = (state->ambient_color & 0xFF) / 255.0f;
    
    /* Add ambient contribution */
    float mat_ambient_r = ((state->current_material.ambient >> 16) & 0xFF) / 255.0f;
    float mat_ambient_g = ((state->current_material.ambient >> 8) & 0xFF) / 255.0f;
    float mat_ambient_b = (state->current_material.ambient & 0xFF) / 255.0f;
    
    r += ambient_r * mat_ambient_r;
    g += ambient_g * mat_ambient_g;
    b += ambient_b * mat_ambient_b;
    
    /* Add emissive contribution */
    float mat_emissive_r = ((state->current_material.emissive >> 16) & 0xFF) / 255.0f;
    float mat_emissive_g = ((state->current_material.emissive >> 8) & 0xFF) / 255.0f;
    float mat_emissive_b = (state->current_material.emissive & 0xFF) / 255.0f;
    
    r += mat_emissive_r;
    g += mat_emissive_g;
    b += mat_emissive_b;
    
    /* Extract material diffuse color */
    float mat_diffuse_r = ((state->current_material.diffuse >> 16) & 0xFF) / 255.0f;
    float mat_diffuse_g = ((state->current_material.diffuse >> 8) & 0xFF) / 255.0f;
    float mat_diffuse_b = (state->current_material.diffuse & 0xFF) / 255.0f;
    
    /* Extract material specular color */
    float mat_specular_r = ((state->current_material.specular >> 16) & 0xFF) / 255.0f;
    float mat_specular_g = ((state->current_material.specular >> 8) & 0xFF) / 255.0f;
    float mat_specular_b = (state->current_material.specular & 0xFF) / 255.0f;
    
    /* Process each light */
    for (int i = 0; i < 8; i++) {
        neon_light_t *light = &state->lights[i];
        
        /* Skip disabled lights */
        if (!light->enabled)
            continue;
        
        float light_r, light_g, light_b;
        float diffuse_factor = 0.0f;
        float specular_factor = 0.0f;
        
        /* Extract light diffuse color */
        light_r = ((light->diffuse >> 16) & 0xFF) / 255.0f;
        light_g = ((light->diffuse >> 8) & 0xFF) / 255.0f;
        light_b = (light->diffuse & 0xFF) / 255.0f;
        
        switch (light->type) {
            case 0: /* Directional light */
                {
                    /* Calculate diffuse lighting */
                    float dot = v->nx * light->direction[0] + 
                                v->ny * light->direction[1] + 
                                v->nz * light->direction[2];
                    
                    if (dot > 0.0f) {
                        diffuse_factor = dot;
                        
                        /* Calculate specular lighting (Phong model) */
                        if (state->current_material.power > 0.0f) {
                            /* Reflection vector */
                            float reflect_x = 2.0f * dot * v->nx - light->direction[0];
                            float reflect_y = 2.0f * dot * v->ny - light->direction[1];
                            float reflect_z = 2.0f * dot * v->nz - light->direction[2];
                            
                            /* View vector (assuming camera at origin, vertex in view space) */
                            float view_x = 0.0f - v->x;
                            float view_y = 0.0f - v->y;
                            float view_z = 0.0f - v->z;
                            
                            /* Normalize view vector */
                            float view_length = sqrtf(view_x*view_x + view_y*view_y + view_z*view_z);
                            if (view_length > 0.0001f) {
                                view_x /= view_length;
                                view_y /= view_length;
                                view_z /= view_length;
                            }
                            
                            /* Calculate specular factor */
                            float spec_dot = reflect_x * view_x + reflect_y * view_y + reflect_z * view_z;
                            if (spec_dot > 0.0f) {
                                specular_factor = powf(spec_dot, state->current_material.power);
                            }
                        }
                    }
                }
                break;
                
            case 1: /* Point light */
                {
                    /* Light to vertex vector */
                    float light_x = light->position[0] - v->x;
                    float light_y = light->position[1] - v->y;
                    float light_z = light->position[2] - v->z;
                    
                    /* Distance from light to vertex */
                    float distance = sqrtf(light_x*light_x + light_y*light_y + light_z*light_z);
                    
                    /* Check if vertex is within light range */
                    if (distance <= light->range) {
                        /* Normalize light vector */
                        if (distance > 0.0001f) {
                            light_x /= distance;
                            light_y /= distance;
                            light_z /= distance;
                        }
                        
                        /* Calculate diffuse lighting */
                        float dot = v->nx * light_x + v->ny * light_y + v->nz * light_z;
                        
                        if (dot > 0.0f) {
                            /* Apply attenuation */
                            float attenuation = 1.0f / (light->attenuation[0] + 
                                                       light->attenuation[1] * distance + 
                                                       light->attenuation[2] * distance * distance);
                            
                            diffuse_factor = dot * attenuation;
                            
                            /* Calculate specular lighting (Phong model) */
                            if (state->current_material.power > 0.0f) {
                                /* Reflection vector */
                                float reflect_x = 2.0f * dot * v->nx - light_x;
                                float reflect_y = 2.0f * dot * v->ny - light_y;
                                float reflect_z = 2.0f * dot * v->nz - light_z;
                                
                                /* View vector (assuming camera at origin, vertex in view space) */
                                float view_x = 0.0f - v->x;
                                float view_y = 0.0f - v->y;
                                float view_z = 0.0f - v->z;
                                
                                /* Normalize view vector */
                                float view_length = sqrtf(view_x*view_x + view_y*view_y + view_z*view_z);
                                if (view_length > 0.0001f) {
                                    view_x /= view_length;
                                    view_y /= view_length;
                                    view_z /= view_length;
                                }
                                
                                /* Calculate specular factor */
                                float spec_dot = reflect_x * view_x + reflect_y * view_y + reflect_z * view_z;
                                if (spec_dot > 0.0f) {
                                    specular_factor = powf(spec_dot, state->current_material.power) * attenuation;
                                }
                            }
                        }
                    }
                }
                break;
                
            case 2: /* Spot light */
                {
                    /* Light to vertex vector */
                    float light_x = light->position[0] - v->x;
                    float light_y = light->position[1] - v->y;
                    float light_z = light->position[2] - v->z;
                    
                    /* Distance from light to vertex */
                    float distance = sqrtf(light_x*light_x + light_y*light_y + light_z*light_z);
                    
                    /* Check if vertex is within light range */
                    if (distance <= light->range) {
                        /* Normalize light vector */
                        if (distance > 0.0001f) {
                            light_x /= distance;
                            light_y /= distance;
                            light_z /= distance;
                        }
                        
                        /* Spot test - make sure vertex is within cone */
                        float cos_angle = -(light_x * light->direction[0] + 
                                          light_y * light->direction[1] + 
                                          light_z * light->direction[2]);
                        
                        if (cos_angle > cosf(light->phi)) {
                            /* Calculate spot attenuation */
                            float spot_attenuation = 1.0f;
                            
                            if (cos_angle < cosf(light->theta)) {
                                /* Between inner and outer cone */
                                spot_attenuation = powf((cos_angle - cosf(light->phi)) / 
                                                      (cosf(light->theta) - cosf(light->phi)), 
                                                      light->falloff);
                            }
                            
                            /* Calculate diffuse lighting */
                            float dot = v->nx * light_x + v->ny * light_y + v->nz * light_z;
                            
                            if (dot > 0.0f) {
                                /* Apply attenuation */
                                float attenuation = 1.0f / (light->attenuation[0] + 
                                                          light->attenuation[1] * distance + 
                                                          light->attenuation[2] * distance * distance);
                                
                                diffuse_factor = dot * attenuation * spot_attenuation;
                                
                                /* Calculate specular lighting (Phong model) */
                                if (state->current_material.power > 0.0f) {
                                    /* Reflection vector */
                                    float reflect_x = 2.0f * dot * v->nx - light_x;
                                    float reflect_y = 2.0f * dot * v->ny - light_y;
                                    float reflect_z = 2.0f * dot * v->nz - light_z;
                                    
                                    /* View vector (assuming camera at origin, vertex in view space) */
                                    float view_x = 0.0f - v->x;
                                    float view_y = 0.0f - v->y;
                                    float view_z = 0.0f - v->z;
                                    
                                    /* Normalize view vector */
                                    float view_length = sqrtf(view_x*view_x + view_y*view_y + view_z*view_z);
                                    if (view_length > 0.0001f) {
                                        view_x /= view_length;
                                        view_y /= view_length;
                                        view_z /= view_length;
                                    }
                                    
                                    /* Calculate specular factor */
                                    float spec_dot = reflect_x * view_x + reflect_y * view_y + reflect_z * view_z;
                                    if (spec_dot > 0.0f) {
                                        specular_factor = powf(spec_dot, state->current_material.power) * 
                                                        attenuation * spot_attenuation;
                                    }
                                }
                            }
                        }
                    }
                }
                break;
        }
        
        /* Add diffuse contribution */
        r += diffuse_factor * light_r * mat_diffuse_r;
        g += diffuse_factor * light_g * mat_diffuse_g;
        b += diffuse_factor * light_b * mat_diffuse_b;
        
        /* Add specular contribution */
        if (specular_factor > 0.0f) {
            float spec_r = ((light->specular >> 16) & 0xFF) / 255.0f;
            float spec_g = ((light->specular >> 8) & 0xFF) / 255.0f;
            float spec_b = (light->specular & 0xFF) / 255.0f;
            
            r += specular_factor * spec_r * mat_specular_r;
            g += specular_factor * spec_g * mat_specular_g;
            b += specular_factor * spec_b * mat_specular_b;
        }
    }
    
    /* Clamp final color values to [0,1] */
    if (r > 1.0f) r = 1.0f;
    if (g > 1.0f) g = 1.0f;
    if (b > 1.0f) b = 1.0f;
    
    /* Convert to 8-bit per channel RGB */
    uint8_t red = (uint8_t)(r * 255.0f);
    uint8_t green = (uint8_t)(g * 255.0f);
    uint8_t blue = (uint8_t)(b * 255.0f);
    
    /* Update vertex color */
    v->color = (v->color & 0xFF000000) | (red << 16) | (green << 8) | blue;
}

/* Setup transformation matrices */
static void neon_3d_setup_matrices(neon250_t *neon250, uint32_t matrix_type, float *matrix)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    switch (matrix_type) {
        case 0: /* World matrix */
            memcpy(state->matrices.world, matrix, 16 * sizeof(float));
            break;
            
        case 1: /* View matrix */
            memcpy(state->matrices.view, matrix, 16 * sizeof(float));
            break;
            
        case 2: /* Projection matrix */
            memcpy(state->matrices.projection, matrix, 16 * sizeof(float));
            break;
            
        case 3: /* Texture matrix */
            memcpy(state->matrices.texture, matrix, 16 * sizeof(float));
            break;
    }
}

/* Configure material properties */
static void neon_3d_configure_material(neon250_t *neon250, neon_material_t *material)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Copy material properties */
    memcpy(&state->current_material, material, sizeof(neon_material_t));
}

/* Setup light parameters */
static void neon_3d_setup_light(neon250_t *neon250, uint32_t light_index, neon_light_t *light)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Copy light parameters */
    memcpy(&state->lights[light_index], light, sizeof(neon_light_t));
}

/* Clear buffers */
static void neon_3d_clear_buffers(neon250_t *neon250, bool clear_color, bool clear_z, uint32_t color, uint16_t z_value)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Clear color buffer if requested */
    if (clear_color) {
        uint32_t display_addr = state->display_addr;
        uint32_t stride = neon250->svga->rowoffset;
        uint32_t width = neon250->svga->hdisp;
        uint32_t height = neon250->svga->vdisp;
        uint32_t bpp = neon250->svga->bpp;
        
        /* Clear based on pixel format */
        switch (bpp) {
            case 8:
                /* 8-bit indexed color */
                memset(&neon250->vram[display_addr], color & 0xFF, width * height);
                break;
                
            case 15:
            case 16:
                /* 16-bit RGB */
                {
                    uint16_t *dest = (uint16_t*)&neon250->vram[display_addr];
                    uint16_t color16;
                    
                    if (bpp == 15) {
                        /* RGB555 */
                        color16 = ((color >> 9) & 0x7C00) |  /* R */
                                  ((color >> 6) & 0x03E0) |  /* G */
                                  ((color >> 3) & 0x001F);   /* B */
                    } else {
                        /* RGB565 */
                        color16 = ((color >> 8) & 0xF800) |  /* R */
                                  ((color >> 5) & 0x07E0) |  /* G */
                                  ((color >> 3) & 0x001F);   /* B */
                    }
                    
                    for (uint32_t y = 0; y < height; y++) {
                        for (uint32_t x = 0; x < width; x++) {
                            dest[x] = color16;
                        }
                        dest += stride;
                    }
                }
                break;
                
            case 24:
                /* 24-bit RGB */
                {
                    uint8_t *dest = &neon250->vram[display_addr];
                    uint8_t r = (color >> 16) & 0xFF;
                    uint8_t g = (color >> 8) & 0xFF;
                    uint8_t b = color & 0xFF;
                    
                    for (uint32_t y = 0; y < height; y++) {
                        for (uint32_t x = 0; x < width; x++) {
                            dest[x*3]   = b;  /* B */
                            dest[x*3+1] = g;  /* G */
                            dest[x*3+2] = r;  /* R */
                        }
                        dest += stride * 3;
                    }
                }
                break;
                
            case 32:
                /* 32-bit ARGB */
                {
                    uint32_t *dest = (uint32_t*)&neon250->vram[display_addr];
                    
                    for (uint32_t y = 0; y < height; y++) {
                        for (uint32_t x = 0; x < width; x++) {
                            dest[x] = color;
                        }
                        dest += stride;
                    }
                }
                break;
        }
    }
    
    /* Clear Z-buffer if requested */
    if (clear_z && state->z_buffer) {
        uint32_t width = neon250->svga->hdisp;
        uint32_t height = neon250->svga->vdisp;
        
        /* Fill Z-buffer with the specified value */
        for (uint32_t i = 0; i < width * height; i++) {
            state->z_buffer[i] = z_value;
        }
    }
}

/* Load texture */
static void neon_3d_load_texture(neon250_t *neon250, uint32_t address, uint16_t width, uint16_t height, uint8_t format)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Setup texture properties */
    state->current_texture.address = address & neon250->vram_mask;
    state->current_texture.width = width;
    state->current_texture.height = height;
    state->current_texture.format = format;
    
    /* Update texture enabled flag */
    state->texture_enabled = true;
}

/* Draw a triangle */
static void neon_3d_draw_triangle(neon250_t *neon250, neon_vertex_t *v1, neon_vertex_t *v2, neon_vertex_t *v3)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Check if we're in wireframe mode */
    if (state->wireframe_mode) {
        /* Draw as wireframe (three lines) */
        neon_3d_draw_line(neon250, v1, v2);
        neon_3d_draw_line(neon250, v2, v3);
        neon_3d_draw_line(neon250, v3, v1);
        return;
    }
    
    /* Screen coordinates */
    int x1 = (int)v1->x;
    int y1 = (int)v1->y;
    int x2 = (int)v2->x;
    int y2 = (int)v2->y;
    int x3 = (int)v3->x;
    int y3 = (int)v3->y;
    
    /* Check if triangle is outside the viewport */
    int vp_left = (state->scissor_x >> 16) & 0xFFFF;
    int vp_right = state->scissor_x & 0xFFFF;
    int vp_top = (state->scissor_y >> 16) & 0xFFFF;
    int vp_bottom = state->scissor_y & 0xFFFF;
    
    if ((x1 < vp_left && x2 < vp_left && x3 < vp_left) ||
        (x1 > vp_right && x2 > vp_right && x3 > vp_right) ||
        (y1 < vp_top && y2 < vp_top && y3 < vp_top) ||
        (y1 > vp_bottom && y2 > vp_bottom && y3 > vp_bottom)) {
        return;
    }
    
    /* Sort vertices by Y coordinate (top to bottom) */
    if (y1 > y2) {
        /* Swap v1 and v2 */
        neon_vertex_t temp = *v1;
        *v1 = *v2;
        *v2 = temp;
        
        int temp_x = x1;
        x1 = x2;
        x2 = temp_x;
        
        int temp_y = y1;
        y1 = y2;
        y2 = temp_y;
    }
    
    if (y2 > y3) {
        /* Swap v2 and v3 */
        neon_vertex_t temp = *v2;
        *v2 = *v3;
        *v3 = temp;
        
        int temp_x = x2;
        x2 = x3;
        x3 = temp_x;
        
        int temp_y = y2;
        y2 = y3;
        y3 = temp_y;
        
        /* Check if we need to swap v1 and v2 again */
        if (y1 > y2) {
            /* Swap v1 and v2 */
            temp = *v1;
            *v1 = *v2;
            *v2 = temp;
            
            temp_x = x1;
            x1 = x2;
            x2 = temp_x;
            
            temp_y = y1;
            y1 = y2;
            y2 = temp_y;
        }
    }
    
    /* Triangle setup would continue here with edge walking and pixel filling */
    /* This would include Z-buffer testing, texture mapping, and blending */
    
    /* This is a simplified implementation - real hardware would use more optimized rasterization */
    
    /* Draw the filled triangle */
    /* For now, we'll use a simple scanline algorithm */
    /* A real implementation would be more complex and optimized */
    
    /* Mark the triangle as rendered */
    state->triangles_rendered++;
}

/* Draw a line */
static void neon_3d_draw_line(neon250_t *neon250, neon_vertex_t *v1, neon_vertex_t *v2)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Screen coordinates */
    int x1 = (int)v1->x;
    int y1 = (int)v1->y;
    int x2 = (int)v2->x;
    int y2 = (int)v2->y;
    
    /* Check if line is outside the viewport */
    int vp_left = (state->scissor_x >> 16) & 0xFFFF;
    int vp_right = state->scissor_x & 0xFFFF;
    int vp_top = (state->scissor_y >> 16) & 0xFFFF;
    int vp_bottom = state->scissor_y & 0xFFFF;
    
    if ((x1 < vp_left && x2 < vp_left) ||
        (x1 > vp_right && x2 > vp_right) ||
        (y1 < vp_top && y2 < vp_top) ||
        (y1 > vp_bottom && y2 > vp_bottom)) {
        return;
    }
    
    /* Line drawing algorithm (Bresenham's) would go here */
    
    /* Mark the line as rendered */
    state->lines_rendered++;
}

/* Draw a point */
static void neon_3d_draw_point(neon250_t *neon250, neon_vertex_t *v)
{
    neon_3d_state_t *state = neon250->state_3d;
    
    /* Screen coordinates */
    int x = (int)v->x;
    int y = (int)v->y;
    
    /* Check if point is inside the viewport */
    int vp_left = (state->scissor_x >> 16) & 0xFFFF;
    int vp_right = state->scissor_x & 0xFFFF;
    int vp_top = (state->scissor_y >> 16) & 0xFFFF;
    int vp_bottom = state->scissor_y & 0xFFFF;
    
    if (x < vp_left || x > vp_right || y < vp_top || y > vp_bottom) {
        return;
    }
    
    /* Point drawing would go here */
    
    /* Mark the point as rendered */
    state->points_rendered++;
}