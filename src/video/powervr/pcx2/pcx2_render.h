/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          PowerVR PCX2 3D rendering pipeline header.
 *
 * Authors: frostbite2000,
 *
 *          Copyright 2025 frostbite2000.
 */

#ifndef VIDEO_POWERVR_PCX2_RENDER_H
#define VIDEO_POWERVR_PCX2_RENDER_H

/* Color structure */
typedef struct {
    uint8_t r;  /* Red component */
    uint8_t g;  /* Green component */
    uint8_t b;  /* Blue component */
} pcx2_rgb_t;

/* Render states */
#define PCX2_RENDER_STATE_IDLE     0     /* No rendering in progress */
#define PCX2_RENDER_STATE_ACTIVE   1     /* Rendering in progress */
#define PCX2_RENDER_STATE_COMPLETE 2     /* Rendering completed */

/* Texture filtering modes */
#define PCX2_FILTER_BILINEAR      0x00
#define PCX2_FILTER_ADAPTIVE      0x01
#define PCX2_FILTER_RESERVED      0x02
#define PCX2_FILTER_POINT_SAMPLE  0x03

/* Exported function prototypes */
void pcx2_init_render_pipeline(void *pcx2_device, uint32_t *texture_mem, uint32_t *param_mem);
void pcx2_shutdown_render_pipeline(void *render_context);

/* Rendering configuration functions */
void pcx2_render_frame(void *render_context, pcx2_rgb_t *frame_buffer, int width, int height, uint32_t *object_list, int object_count);
void pcx2_set_render_camera_scale(void *render_context, uint16_t scale);
void pcx2_set_render_fog_color(void *render_context, uint8_t r, uint8_t g, uint8_t b);
void pcx2_set_render_texture_filtering(void *render_context, uint8_t mode);
void pcx2_set_render_fog_enable(void *render_context, uint8_t enable);
void pcx2_set_render_dithering(void *render_context, uint8_t enable);

/* Statistics retrieval function */
void pcx2_get_render_stats(void *render_context, uint32_t *frames, uint32_t *pixels, 
                          uint32_t *tiles, uint32_t *cache_hits, uint32_t *cache_misses);

#endif /* VIDEO_POWERVR_PCX2_RENDER_H */