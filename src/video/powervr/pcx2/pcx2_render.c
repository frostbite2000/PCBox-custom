/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          PowerVR PCX2 3D rendering pipeline implementation.
 *          Based on the PowerVR Series 1 "Texas" rendering core.
 *
 * Authors: [Your name here],
 *
 *          Copyright 2020-2025 [Your name here].
 */

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <math.h>
#define HAVE_STDARG_H

#include "86box/86box.h"
#include "86box/mem.h"
#include "86box/timer.h"
#include "86box/device.h"
#include "86box/plat.h"
#include "86box/video.h"

/* Include header files from the PowerVR PCX2 core */
#include "rectable.h"
#include "fogtable.h"

#include "pcx2_render.h"

/* Define PCX2-specific rendering parameters */
#define PCX2_MAX_X_DIM           2048
#define PCX2_MAX_Y_DIM           2048
#define PCX2_TEXTURE_MEMORY_SIZE (4 * 1024 * 1024)  /* 4MB texture memory */
#define PCX2_BIG_BANK            0x00800000         /* 8MB offset for second bank */
#define PCX2_CACHE_ENTRIES       8                  /* Cache entries for texture cache */
#define PCX2_CACHE_BLOCK_SIZE    7                  /* 128 byte cache blocks (2^7) */

/* Define texture mapping parameters */
#define PCX2_MASK_TEXTURE       0x00000001
#define PCX2_MASK_SMOOTH_SHADE  0x00000002
#define PCX2_MASK_SHADOW_FLAG   0x00000004
#define PCX2_MASK_FLAT_HIGHLIGHT 0x00000008
#define PCX2_MASK_DISABLE_FOG   0x00000010
#define PCX2_MASK_TRANS         0x00000020
#define PCX2_MASK_GLOBAL_TRANS  0x000003C0
#define PCX2_MASK_EXPONENT      0x0000F000
#define PCX2_MASK_FLIP_UV       0x00030000
#define PCX2_MASK_8_16_MAPS     0x00800000
#define PCX2_MASK_MIP_MAPPED    0x01000000
#define PCX2_MASK_MAP_SIZE      0x0C000000
#define PCX2_MASK_4444_555      0x10000000
#define PCX2_MASK_PMIP_M        0xFF000000
#define PCX2_MASK_PMIP_E        0x00F00000

/* Define bit shifts for parameter extraction */
#define PCX2_SHIFT_GLOBAL_TRANS 6
#define PCX2_SHIFT_EXPONENT     12
#define PCX2_SHIFT_FLIP_UV      16
#define PCX2_SHIFT_MAP_SIZE     26
#define PCX2_SHIFT_PMIP_M       24
#define PCX2_SHIFT_PMIP_E       20

/* PCX2 Pixel Format Definitions */
#define PCX2_PIXEL_FORMAT_RGB555  0x00
#define PCX2_PIXEL_FORMAT_RGB565  0x01
#define PCX2_PIXEL_FORMAT_ARGB555 0x02
#define PCX2_PIXEL_FORMAT_ARGB444 0x03
#define PCX2_PIXEL_FORMAT_RGB332  0x04

/* PCX2 Texture Filtering Modes */
#define PCX2_FILTER_BILINEAR      0x00
#define PCX2_FILTER_ADAPTIVE      0x01
#define PCX2_FILTER_RESERVED      0x02
#define PCX2_FILTER_POINT_SAMPLE  0x03

/* Texture Size Definitions */
#define PCX2_TEXTURE_SIZE_256x256 0x00
#define PCX2_TEXTURE_SIZE_128x128 0x01
#define PCX2_TEXTURE_SIZE_64x64   0x02
#define PCX2_TEXTURE_SIZE_32x32   0x03

/* Define structure for floating point representation */
typedef struct {
    int32_t m;  /* Mantissa */
    int     e;  /* Exponent */
} pcx2_pfloat_t;

typedef struct {
    uint8_t r;  /* Red component */
    uint8_t g;  /* Green component */
    uint8_t b;  /* Blue component */
    uint8_t a;  /* Alpha component */
} pcx2_rgba_t;

/* Define PCX2 tile structure */
typedef struct {
    int x;           /* X-coordinate of tile */
    int y;           /* Y-coordinate of tile */
    int width;       /* Width of tile */
    int height;      /* Height of tile */
    uint32_t param_addr; /* Parameter address for this tile */
} pcx2_tile_t;

/* Define PCX2 texture cache entry */
typedef struct {
    uint32_t address;    /* Texture address */
    uint32_t size;       /* Texture size */
    uint8_t  valid;      /* Valid flag */
    uint8_t  dirty;      /* Dirty flag */
    uint8_t  lru;        /* Least Recently Used counter */
} pcx2_texture_cache_entry_t;

/* Define PCX2 rendering context */
typedef struct {
    /* Frame buffer */
    pcx2_rgb_t *frame_buffer;      /* Frame buffer memory */
    int         frame_width;       /* Width of frame buffer */
    int         frame_height;      /* Height of frame buffer */
    
    /* Texture memory */
    uint32_t   *texture_memory;    /* Texture memory array (32-bit words) */
    uint32_t    texture_mem_size;  /* Size of texture memory in bytes */
    
    /* Parameter memory */
    uint32_t   *param_memory;      /* Parameter memory for geometry data */
    uint32_t    param_mem_size;    /* Size of parameter memory in bytes */
    
    /* Rendering state */
    int         render_state;      /* Current rendering state */
    uint32_t    frame_count;       /* Number of frames rendered */
    uint64_t    render_start_time; /* Time when rendering started */
    uint64_t    render_end_time;   /* Time when rendering completed */
    
    /* Rendering settings */
    pcx2_rgb_t  fog_color;         /* Fog color */
    uint16_t    camera_scale;      /* Camera scale factor */
    uint8_t     texture_filter;    /* Texture filtering mode */
    uint8_t     dithering;         /* Dithering enabled flag */
    uint8_t     fog_enable;        /* Fog enabled flag */
    
    /* Statistics */
    uint32_t    pixels_rendered;   /* Number of pixels rendered */
    uint32_t    tiles_processed;   /* Number of tiles processed */
    uint32_t    texture_cache_hits;    /* Number of texture cache hits */
    uint32_t    texture_cache_misses;  /* Number of texture cache misses */
    
    /* Texture cache */
    pcx2_texture_cache_entry_t texture_cache[16];  /* Texture cache entries */
} pcx2_render_context_t;

/* Function prototypes */
static void pcx2_init_render_context(pcx2_render_context_t *ctx, uint32_t *texture_mem, uint32_t *param_mem);
static void pcx2_reset_render_context(pcx2_render_context_t *ctx);
static void pcx2_start_frame(pcx2_render_context_t *ctx);
static void pcx2_end_frame(pcx2_render_context_t *ctx);
static void pcx2_process_tile(pcx2_render_context_t *ctx, pcx2_tile_t *tile);
static void pcx2_render_pixel(pcx2_render_context_t *ctx, int x, int y, uint32_t param_addr, uint8_t shadow, uint8_t fog);
static pcx2_rgba_t pcx2_texture_pixel(pcx2_render_context_t *ctx, int x, int y, 
                                     int a, int b, int c, int d, int e, int f, 
                                     int p, int q, int r, int exp, uint32_t tex_addr, 
                                     uint8_t map_size, pcx2_pfloat_t pmip, 
                                     uint8_t color_depth, uint8_t mip_mapped, 
                                     uint8_t col4444or555, int global_trans, 
                                     uint8_t flip_uv, uint8_t translucent);
static uint32_t pcx2_address_calc(pcx2_render_context_t *ctx, int32_t u, int32_t v, 
                                 uint32_t address, uint8_t map_size, int which_map, 
                                 uint8_t color_depth, uint8_t mip_mapped, uint8_t flip_uv);
static uint16_t pcx2_fetch_pixel(pcx2_render_context_t *ctx, uint32_t address);
static pcx2_rgba_t pcx2_convert_pixel_format(uint16_t raw_pixel, uint8_t color_depth, 
                                           uint8_t which_pixel, uint8_t translucent);
static pcx2_pfloat_t pcx2_to_pfloat(int32_t x);
static int32_t pcx2_arithmetic_shift(int32_t x, int shift);
static uint32_t pcx2_twiddle_coordinate(uint32_t u, uint32_t v);
static int32_t pcx2_linear_shade(int t0, int t1, int t2, int x_local, int y_local);

/* Define the actual reciprocal table data */
const uint16_t pcx2_reciprocal_table[256] = {
    /* 1/x values in 0.16 fixed point format */
    0xFFFF, 0x8000, 0x5555, 0x4000, 0x3333, 0x2AAA, 0x2492, 0x2000, /* 1/1 through 1/8 */
    0x1C71, 0x1999, 0x1745, 0x1555, 0x13B1, 0x1249, 0x1111, 0x1000, /* 1/9 through 1/16 */
    0x0F0F, 0x0E38, 0x0D79, 0x0CCC, 0x0C30, 0x0BA2, 0x0B21, 0x0AAA, /* 1/17 through 1/24 */
    0x0A3D, 0x09D8, 0x097B, 0x0924, 0x08D3, 0x0888, 0x0842, 0x0800, /* 1/25 through 1/32 */
    0x07C1, 0x0787, 0x0750, 0x071C, 0x06EB, 0x06BC, 0x0690, 0x0666, /* 1/33 through 1/40 */
    0x063E, 0x0618, 0x05F4, 0x05D1, 0x05B0, 0x0590, 0x0572, 0x0555, /* 1/41 through 1/48 */
    0x0539, 0x051E, 0x0505, 0x04EC, 0x04D4, 0x04BD, 0x04A7, 0x0492, /* 1/49 through 1/56 */
    0x047D, 0x0469, 0x0456, 0x0444, 0x0432, 0x0421, 0x0410, 0x0400, /* 1/57 through 1/64 */
    0x03F0, 0x03E0, 0x03D2, 0x03C3, 0x03B5, 0x03A8, 0x039A, 0x038E, /* 1/65 through 1/72 */
    0x0381, 0x0375, 0x0369, 0x035E, 0x0353, 0x0348, 0x033D, 0x0333, /* 1/73 through 1/80 */
    0x0329, 0x031F, 0x0315, 0x030C, 0x0303, 0x02FA, 0x02F1, 0x02E8, /* 1/81 through 1/88 */
    0x02E0, 0x02D8, 0x02D0, 0x02C8, 0x02C0, 0x02B9, 0x02B1, 0x02AA, /* 1/89 through 1/96 */
    0x02A3, 0x029C, 0x0295, 0x028F, 0x0288, 0x0282, 0x027C, 0x0276, /* 1/97 through 1/104 */
    0x0270, 0x026A, 0x0265, 0x025F, 0x025A, 0x0254, 0x024F, 0x024A, /* 1/105 through 1/112 */
    0x0245, 0x0240, 0x023B, 0x0236, 0x0231, 0x022D, 0x0228, 0x0224, /* 1/113 through 1/120 */
    0x021F, 0x021B, 0x0217, 0x0213, 0x020F, 0x020B, 0x0207, 0x0203, /* 1/121 through 1/128 */
    0x01FF, 0x01FB, 0x01F7, 0x01F4, 0x01F0, 0x01EC, 0x01E9, 0x01E5, /* 1/129 through 1/136 */
    0x01E2, 0x01DE, 0x01DB, 0x01D8, 0x01D4, 0x01D1, 0x01CE, 0x01CB, /* 1/137 through 1/144 */
    0x01C8, 0x01C5, 0x01C2, 0x01BF, 0x01BC, 0x01B9, 0x01B6, 0x01B4, /* 1/145 through 1/152 */
    0x01B1, 0x01AE, 0x01AC, 0x01A9, 0x01A6, 0x01A4, 0x01A1, 0x019F, /* 1/153 through 1/160 */
    0x019C, 0x019A, 0x0197, 0x0195, 0x0192, 0x0190, 0x018E, 0x018C, /* 1/161 through 1/168 */
    0x0189, 0x0187, 0x0185, 0x0183, 0x0181, 0x017F, 0x017D, 0x017B, /* 1/169 through 1/176 */
    0x0179, 0x0177, 0x0175, 0x0173, 0x0171, 0x016F, 0x016D, 0x016B, /* 1/177 through 1/184 */
    0x0169, 0x0168, 0x0166, 0x0164, 0x0162, 0x0161, 0x015F, 0x015D, /* 1/185 through 1/192 */
    0x015B, 0x015A, 0x0158, 0x0157, 0x0155, 0x0154, 0x0152, 0x0151, /* 1/193 through 1/200 */
    0x014F, 0x014E, 0x014C, 0x014B, 0x0149, 0x0148, 0x0146, 0x0145, /* 1/201 through 1/208 */
    0x0143, 0x0142, 0x0141, 0x013F, 0x013E, 0x013D, 0x013B, 0x013A, /* 1/209 through 1/216 */
    0x0139, 0x0138, 0x0136, 0x0135, 0x0134, 0x0133, 0x0131, 0x0130, /* 1/217 through 1/224 */
    0x012F, 0x012E, 0x012D, 0x012B, 0x012A, 0x0129, 0x0128, 0x0127, /* 1/225 through 1/232 */
    0x0126, 0x0125, 0x0124, 0x0122, 0x0121, 0x0120, 0x011F, 0x011E, /* 1/233 through 1/240 */
    0x011D, 0x011C, 0x011B, 0x011A, 0x0119, 0x0118, 0x0117, 0x0116, /* 1/241 through 1/248 */
    0x0115, 0x0114, 0x0113, 0x0112, 0x0111, 0x0110, 0x010F, 0x010F  /* 1/249 through 1/256 */
};

/* 
 * Texture coordinate twiddle function
 * This function interleaves the bits of the u and v coordinates to 
 * generate a memory address, creating a Z-order curve (Morton code)
 * Based on the original PowerVR Series1 implementation
 */
static uint32_t 
pcx2_twiddle_coordinate(uint32_t u, uint32_t v)
{
    uint32_t result = 0;
    uint32_t i;
    
    /* Interleave bits from u and v to create the twiddle pattern */
    for (i = 0; i < 8; i++) {
        result |= ((u & (1 << i)) << i) | ((v & (1 << i)) << (i + 1));
    }
    
    return result;
}

/*
 * Convert integer to pfloat
 * Returns the pcx2_pfloat_t representation of a 32-bit integer
 * Based on the PowerVR Series1 ToPfloat function
 */
static pcx2_pfloat_t 
pcx2_to_pfloat(int32_t x)
{
    pcx2_pfloat_t temp;
    
    /* Find the position of the top bit of x */
    temp.e = 0;
    
    if (x < 0) {
        for (temp.e = 0; (1 << temp.e) <= -x; temp.e++);
    } else if (x > 0) {
        for (temp.e = 0; (1 << temp.e) <= x; temp.e++);
    }
    
    /* Convert to mantissa-exponent format */
    temp.m = x << (31 - temp.e);
    temp.m >>= 16;
    
    return temp;
}

/*
 * Arithmetic shift function
 * Returns x >> shift, allowing for negative shifts and shifts greater than 31
 * Based on the PowerVR Series1 AShift function
 */
static int32_t 
pcx2_arithmetic_shift(int32_t x, int shift)
{
    if (shift < 0) {
        /* Left shift */
        if (shift < -31) {
            /* Large left shifts result in 0 */
            x = 0;
        } else {
            x <<= -shift;
        }
    } else {
        /* Right shift */
        if (shift > 31) {
            /* Large right shifts: -1 for negative, 0 for positive */
            x = (x < 0) ? -1 : 0;
        } else {
            /* Arithmetic right shift preserves sign bit */
            x >>= shift;
        }
    }
    
    return x;
}

/*
 * Address calculation for texture access
 * Calculate a pixel address given the u,v coordinates, base address, etc.
 * Based on the PowerVR Series1 AddressCalc function
 */
static uint32_t 
pcx2_address_calc(pcx2_render_context_t *ctx, int32_t u, int32_t v, 
                 uint32_t address, uint8_t map_size, int which_map, 
                 uint8_t color_depth, uint8_t mip_mapped, uint8_t flip_uv)
{
    uint32_t pixel_address;
    
    /* Mip-map offsets for different texture sizes */
    static const uint32_t mip_map_offset[] = {
        0x5555, 0x1555, 0x0555, 0x0155, 
        0x0055, 0x0015, 0x0005, 0x0001, 0x0000
    };
    
    /* Masks for texture wrapping at different sizes */
    static const uint32_t map_mask[] = {
        0x00ff, /* 256x256 */
        0x007f, /* 128x128 */
        0x003f, /* 64x64 */
        0x001f  /* 32x32 */
    };
    
    /* Handle texture flipping if enabled */
    if (map_size && flip_uv) {
        /* If the map size is not 256 and either U or V are to be reflected */
        if (((map_mask[map_size] + 1) & u) && (flip_uv & 2))
            u = ~u;
        if (((map_mask[map_size] + 1) & v) && (flip_uv & 1))
            v = ~v;
    } else if (map_size == 0 && flip_uv) {
        /* If the map size is 256, flip as if it is 128 */
        if ((128 & u) && (flip_uv & 2))
            u = ~u;
        if ((128 & v) && (flip_uv & 1))
            v = ~v;
    }
    
    /* Apply texture coordinate wrapping */
    u &= map_mask[map_size];
    v &= map_mask[map_size];
    
    if (mip_mapped == 0) {
        /* Non-mipmapped textures */
        if (color_depth == 1) {
            /* 16-bit textures */
            pixel_address = address + pcx2_twiddle_coordinate(u, v);
        } else {
            /* 8-bit textures (two pixels per 16-bit word) */
            pixel_address = address + pcx2_twiddle_coordinate(u >> 1, v);
        }
    } else {
        /* Mipmapped textures */
        
        /* If odd map, toggle big bank */
        if ((map_size + which_map) & 1)
            address ^= PCX2_BIG_BANK;
        
        /* Calculate address using mip-map offsets and twiddle function */
        pixel_address = address + mip_map_offset[map_size + which_map] + 
                       pcx2_twiddle_coordinate(u >> which_map, v >> which_map);
    }
    
    return pixel_address;
}

/*
 * Texture pixel fetch function
 * Returns a raw 16-bit pixel given a memory address
 * Based on the PowerVR Series1 FetchPixel function
 */
static uint16_t 
pcx2_fetch_pixel(pcx2_render_context_t *ctx, uint32_t address)
{
    uint16_t pixel = 0;
    uint32_t pixel_word;
    
    /* Check if address is in valid range */
    if (address < (ctx->texture_mem_size / 2)) {
        /* First bank */
        pixel_word = ctx->texture_memory[address >> 1];
    } else if ((address >= PCX2_BIG_BANK) && 
               (address < (PCX2_BIG_BANK + (ctx->texture_mem_size / 2)))) {
        /* Second bank */
        pixel_word = ctx->texture_memory[((address ^ PCX2_BIG_BANK) + 
                                        (ctx->texture_mem_size / 2)) >> 1];
    } else {
        /* Invalid address */
        pclog("PCX2 Render: Invalid texture address: 0x%08X\n", address);
        return 0;
    }
    
    /* Extract the 16-bit pixel from the 32-bit word */
    if (address & 1) {
        /* Lower 16 bits */
        pixel = pixel_word & 0xFFFF;
    } else {
        /* Upper 16 bits */
        pixel = pixel_word >> 16;
    }
    
    /* Update statistics */
    ctx->texture_cache_hits++;
    
    return pixel;
}

/*
 * Color format conversion function
 * Converts a raw pixel value to RGBA format based on the color depth
 * Based on the PowerVR Series1 ColourConvert function
 */
static pcx2_rgba_t 
pcx2_convert_pixel_format(uint16_t raw_pixel, uint8_t color_depth, 
                         uint8_t which_pixel, uint8_t translucent)
{
    pcx2_rgba_t pixel;
    
    if (translucent) {
        /* ARGB4444 format */
        pixel.b = (raw_pixel & 0x000F);
        pixel.g = (raw_pixel & 0x00F0) >> 4;
        pixel.r = (raw_pixel & 0x0F00) >> 8;
        pixel.a = (raw_pixel & 0xF000) >> 12;
        
        /* Expand to 8-bit precision */
        pixel.r = (pixel.r << 4) | pixel.r;
        pixel.g = (pixel.g << 4) | pixel.g;
        pixel.b = (pixel.b << 4) | pixel.b;
        pixel.a = (pixel.a << 4) | pixel.a;
    } else if (color_depth) {
        /* RGB555 format */
        pixel.r = (raw_pixel >> 10) & 0x1F;
        pixel.g = (raw_pixel >> 5) & 0x1F;
        pixel.b = raw_pixel & 0x1F;
        pixel.a = 0;
        
        /* Expand to 8-bit precision */
        pixel.r = (pixel.r << 3) | (pixel.r >> 2);
        pixel.g = (pixel.g << 3) | (pixel.g >> 2);
        pixel.b = (pixel.b << 3) | (pixel.b >> 2);
    } else {
        /* RGB332 format */
        /* Select high or low byte */
        if (which_pixel == 0)
            raw_pixel >>= 8;
            
        pixel.r = raw_pixel & 0xE0;
        pixel.g = (raw_pixel << 3) & 0xE0;
        pixel.b = (raw_pixel << 6) & 0xC0;
        pixel.a = 0;
        
        /* Expand by repeating bits */
        if (pixel.b & 0x80)
            pixel.b |= 0x3F;
        if (pixel.g & 0x80)
            pixel.g |= 0x1F;
        if (pixel.r & 0x80)
            pixel.r |= 0x1F;
            
        /* Convert to 8-bit precision */
        pixel.r = (pixel.r * 255) / 31;
        pixel.g = (pixel.g * 255) / 31;
        pixel.b = (pixel.b * 255) / 31;
    }
    
    return pixel;
}

/*
 * Texture pixel lookup function
 * Performs texture mapping with optional mipmapping and filtering
 * Based on the PowerVR Series1 TexturePixel function
 */
static pcx2_rgba_t 
pcx2_texture_pixel(pcx2_render_context_t *ctx, int x, int y, 
                  int a, int b, int c, int d, int e, int f, 
                  int p, int q, int r, int exp, uint32_t tex_addr, 
                  uint8_t map_size, pcx2_pfloat_t pmip, 
                  uint8_t color_depth, uint8_t mip_mapped, 
                  uint8_t col4444or555, int global_trans, 
                  uint8_t flip_uv, uint8_t translucent)
{
    pcx2_rgba_t tcol, high_pixel, low_pixel;
    int32_t abc, def, pqr;
    int shift, power_two;
    pcx2_pfloat_t top, bot;
    int32_t u_frac, v_frac;
    int32_t u, v;
    uint32_t high_res_addr, low_res_addr;
    uint16_t raw_pixel;
    
    /* Calculate texture coordinate */
    abc = a * x + b * y + c * ctx->camera_scale;
    def = d * x + e * y + f * ctx->camera_scale;
    pqr = p * x + q * y + r * ctx->camera_scale;
    
    /* Convert to floating point */
    bot = pcx2_to_pfloat(pqr);
    bot.m >>= 1; /* Reduce precision */
    
    /* Calculate reciprocal */
    if (bot.m > 0)
        bot.m = 0x8000000 / bot.m;
    else
        bot.m = 0x4000;
        
    power_two = (bot.m == 0x4000);
    
    if (power_two)
        bot.m = 0x2000;
        
    /* Calculate U coordinate */
    top = pcx2_to_pfloat(abc);
    top.e += exp; /* Add exponent */
    
    u = (top.m * bot.m) >> 14;
    
    if (power_two)
        shift = top.e - (bot.e + 13);
    else
        shift = top.e - (bot.e + 14);
        
    u_frac = pcx2_arithmetic_shift(u, shift + 5) & 8191;
    u = pcx2_arithmetic_shift(u, shift) & 255;
    
    /* Calculate V coordinate */
    top = pcx2_to_pfloat(def);
    top.e += exp; /* Add exponent */
    
    v = (top.m * bot.m) >> 14;
    
    if (power_two)
        shift = top.e - (bot.e + 13);
    else
        shift = top.e - (bot.e + 14);
        
    v_frac = pcx2_arithmetic_shift(v, shift + 5) & 8191;
    v = pcx2_arithmetic_shift(v, shift) & 255;
    
    /* Calculate mipmap level (d) */
    bot.m >>= 6; /* Reduce resolution to 8 bits */
    bot.m = bot.m * bot.m; /* Square the bottom */
    
    /* Normalize */
    if (power_two)
        bot.e--;
        
    if (bot.m & 0x8000) {
        bot.e = bot.e * 2;
        bot.m >>= 8;
    } else {
        bot.e = bot.e * 2 + 1;
        bot.m >>= 7;
    }
    
    /* Multiply by pmip */
    bot.m = bot.m * pmip.m;
    
    if (bot.m & 0x8000) {
        bot.e = pmip.e - (bot.e - 2);
        bot.m >>= 8;
    } else {
        bot.e = pmip.e - (bot.e - 1);
        bot.m >>= 7;
    }
    
    /* Clamp exponent */
    if (bot.e < 0)
        bot.e = 0;
    if (bot.e > 15)
        bot.e = 15;
        
    /* Convert map_size to internal format */
    map_size = 3 - map_size; /* Flip 00=32x32 to 00=256x256 */
    
    /* Determine which mipmap level to use */
    if (bot.e < 1 || mip_mapped == 0 || color_depth == 0) {
        /* Use highest resolution map */
        high_res_addr = pcx2_address_calc(ctx, u, v, tex_addr, map_size, 0, 
                                         color_depth, mip_mapped, flip_uv);
        
        /* Fetch the pixel and convert to RGBA */
        raw_pixel = pcx2_fetch_pixel(ctx, high_res_addr);
        tcol = pcx2_convert_pixel_format(raw_pixel, color_depth, u & 1, col4444or555);
        
        /* Apply bilinear filtering if enabled */
        if (ctx->texture_filter == PCX2_FILTER_BILINEAR) {
            pcx2_rgba_t u1, v1, u1v1;
            int32_t blend_u, blend_v;
            
            /* Fetch surrounding texels */
            high_res_addr = pcx2_address_calc(ctx, (u + 1) & 255, v, tex_addr, 
                                             map_size, 0, color_depth, mip_mapped, flip_uv);
            raw_pixel = pcx2_fetch_pixel(ctx, high_res_addr);
            u1 = pcx2_convert_pixel_format(raw_pixel, color_depth, (u + 1) & 1, col4444or555);
            
            high_res_addr = pcx2_address_calc(ctx, u, (v + 1) & 255, tex_addr, 
                                             map_size, 0, color_depth, mip_mapped, flip_uv);
            raw_pixel = pcx2_fetch_pixel(ctx, high_res_addr);
            v1 = pcx2_convert_pixel_format(raw_pixel, color_depth, u & 1, col4444or555);
            
            high_res_addr = pcx2_address_calc(ctx, (u + 1) & 255, (v + 1) & 255, tex_addr, 
                                             map_size, 0, color_depth, mip_mapped, flip_uv);
            raw_pixel = pcx2_fetch_pixel(ctx, high_res_addr);
            u1v1 = pcx2_convert_pixel_format(raw_pixel, color_depth, (u + 1) & 1, col4444or555);
            
            /* Calculate blend factors (0-31) */
            blend_u = (u_frac >> 8) & 0x1F;
            blend_v = (v_frac >> 8) & 0x1F;
            
            /* Perform bilinear interpolation */
            tcol.r = tcol.r + (((u1.r - tcol.r) * blend_u) >> 5) + 
                               (((v1.r - tcol.r) * blend_v) >> 5) + 
                               (((u1v1.r - v1.r - u1.r + tcol.r) * blend_u * blend_v) >> 10);
                               
            tcol.g = tcol.g + (((u1.g - tcol.g) * blend_u) >> 5) + 
                               (((v1.g - tcol.g) * blend_v) >> 5) + 
                               (((u1v1.g - v1.g - u1.g + tcol.g) * blend_u * blend_v) >> 10);
                               
            tcol.b = tcol.b + (((u1.b - tcol.b) * blend_u) >> 5) + 
                               (((v1.b - tcol.b) * blend_v) >> 5) + 
                               (((u1v1.b - v1.b - u1.b + tcol.b) * blend_u * blend_v) >> 10);
                               
            tcol.a = tcol.a + (((u1.a - tcol.a) * blend_u) >> 5) + 
                               (((v1.a - tcol.a) * blend_v) >> 5) + 
                               (((u1v1.a - v1.a - u1.a + tcol.a) * blend_u * blend_v) >> 10);
        }
    } else if (map_size + bot.e > 8) {
        /* Use lowest resolution map (1x1) */
        low_res_addr = pcx2_address_calc(ctx, u, v, tex_addr, 0, 8, 
                                        color_depth, mip_mapped, flip_uv);
                                        
        /* Fetch the pixel and convert to RGBA */
        raw_pixel = pcx2_fetch_pixel(ctx, low_res_addr);
        tcol = pcx2_convert_pixel_format(raw_pixel, color_depth, 0, col4444or555);
    } else {
        /* Use interpolation between two mipmap levels */
        int mip_level = bot.e - 1;
        
        /* Fetch pixel from higher resolution mipmap */
        high_res_addr = pcx2_address_calc(ctx, u, v, tex_addr, map_size, mip_level, 
                                         color_depth, mip_mapped, flip_uv);
        raw_pixel = pcx2_fetch_pixel(ctx, high_res_addr);
        high_pixel = pcx2_convert_pixel_format(raw_pixel, color_depth, u & 1, col4444or555);
        
        /* Fetch pixel from lower resolution mipmap */
        low_res_addr = pcx2_address_calc(ctx, u, v, tex_addr, map_size, mip_level + 1, 
                                        color_depth, mip_mapped, flip_uv);
        raw_pixel = pcx2_fetch_pixel(ctx, low_res_addr);
        low_pixel = pcx2_convert_pixel_format(raw_pixel, color_depth, u & 1, col4444or555);
        
        /* Interpolate between the two mipmaps */
        bot.m &= 0x7F; /* Same as subtracting 128 */
        bot.m >>= 2;   /* Reduce resolution by 2 bits */
        
        tcol.r = high_pixel.r + ((bot.m * (low_pixel.r - high_pixel.r)) >> 5);
        tcol.g = high_pixel.g + ((bot.m * (low_pixel.g - high_pixel.g)) >> 5);
        tcol.b = high_pixel.b + ((bot.m * (low_pixel.b - high_pixel.b)) >> 5);
        tcol.a = low_pixel.a;  /* Use lower resolution mipmap's alpha */
    }
    
    /* Add global translucency */
    tcol.a += global_trans;
    if (tcol.a > 15)
        tcol.a = 15;
        
    return tcol;
}

/*
 * Linear shading calculation
 * Calculates the intensity of the shading function at a local coordinate
 * Based on the PowerVR Series1 LinearShade function
 */
static int32_t 
pcx2_linear_shade(int t0, int t1, int t2, int x_local, int y_local)
{
    int32_t intensity;
    
    intensity = t0 << 2;
    intensity += (t1 * y_local);
    intensity += (t2 * x_local);
    
    return intensity;
}

/*
 * Pixel rendering function
 * Renders a single pixel using the specified parameters
 * Based on the PowerVR Series1 Texas function
 */
static void 
pcx2_render_pixel(pcx2_render_context_t *ctx, int x, int y, uint32_t param_addr, uint8_t shadow, uint8_t fog)
{
    pcx2_rgba_t color;
    pcx2_rgb_t base, hold_col, hold_col1, shadow_color, highlight_col, shadow_highlight_col;
    int red, green, blue;
    int32_t fraction;
    int t0, t1, t2;
    int a, b, c, d, e, f, p, q, r;
    int exp, global_trans;
    int x_offset, y_offset;
    uint32_t texture_addr;
    uint8_t map_size, color_depth, mip_mapped, trans_pass, col4444or555, flip_uv;
    pcx2_pfloat_t pmip;
    uint32_t inc_addr;
    
    /* Check if we're out of bounds */
    if (x < 0 || y < 0 || x >= ctx->frame_width || y >= ctx->frame_height)
        return;
        
    inc_addr = param_addr;
    
    /* Get basic parameters */
    uint32_t params = ctx->param_memory[param_addr];
    
    /* Unpack base color or offsets */
    if (params & PCX2_MASK_SMOOTH_SHADE) {
        /* Smooth shaded */
        x_offset = (int16_t)(ctx->param_memory[param_addr + 1] >> 16);
        y_offset = (int16_t)(ctx->param_memory[param_addr + 1]);
    } else {
        /* Flat shaded */
        base.r = params & 0xFF;
        base.g = (ctx->param_memory[param_addr + 1] >> 24) & 0xFF;
        base.b = (ctx->param_memory[param_addr + 1] >> 16) & 0xFF;
        
        /* Extract shadow color */
        uint16_t shadow_raw = ctx->param_memory[param_addr + 1] & 0xFFFF;
        shadow_color.r = ((shadow_raw >> 10) & 0x1F) << 3;
        shadow_color.g = ((shadow_raw >> 5) & 0x1F) << 3;
        shadow_color.b = (shadow_raw & 0x1F) << 3;
        
        /* Apply shadow if needed */
        if ((params & PCX2_MASK_SHADOW_FLAG) && (shadow == 0)) {
            red = base.r + shadow_color.r;
            green = base.g + shadow_color.g;
            blue = base.b + shadow_color.b;
            
            /* Clamp to 255 */
            base.r = (red > 255) ? 255 : red;
            base.g = (green > 255) ? 255 : green;
            base.b = (blue > 255) ? 255 : blue;
        }
    }
    
    /* Perform texture mapping if enabled */
    if (params & PCX2_MASK_TEXTURE) {
        /* Extract texture parameters */
        a = (int16_t)(ctx->param_memory[param_addr + 5]);
        b = (int16_t)(ctx->param_memory[param_addr + 5] >> 16);
        c = (int16_t)(ctx->param_memory[param_addr + 4]);
        
        d = (int16_t)(ctx->param_memory[param_addr + 7]);
        e = (int16_t)(ctx->param_memory[param_addr + 7] >> 16);
        f = (int16_t)(ctx->param_memory[param_addr + 6]);
        
        p = (int16_t)(ctx->param_memory[param_addr + 3]);
        q = (int16_t)(ctx->param_memory[param_addr + 3] >> 16);
        r = (int16_t)(ctx->param_memory[param_addr + 2]);
        
        exp = (params & PCX2_MASK_EXPONENT) >> PCX2_SHIFT_EXPONENT;
        global_trans = (params & PCX2_MASK_GLOBAL_TRANS) >> PCX2_SHIFT_GLOBAL_TRANS;
        flip_uv = (params & PCX2_MASK_FLIP_UV) >> PCX2_SHIFT_FLIP_UV;
        trans_pass = (params & PCX2_MASK_TRANS) > 0;
        
        texture_addr = (ctx->param_memory[param_addr + 4] >> 16) | 
                       (ctx->param_memory[param_addr + 6] & 0x00FF0000);
        
        map_size = (ctx->param_memory[param_addr + 6] & PCX2_MASK_MAP_SIZE) >> PCX2_SHIFT_MAP_SIZE;
        
        pmip.m = (ctx->param_memory[param_addr + 2] & PCX2_MASK_PMIP_M) >> PCX2_SHIFT_PMIP_M;
        pmip.e = (ctx->param_memory[param_addr + 2] & PCX2_MASK_PMIP_E) >> PCX2_SHIFT_PMIP_E;
        
        color_depth = (ctx->param_memory[param_addr + 6] & PCX2_MASK_8_16_MAPS) > 0;
        mip_mapped = (ctx->param_memory[param_addr + 6] & PCX2_MASK_MIP_MAPPED) > 0;
        col4444or555 = (ctx->param_memory[param_addr + 6] & PCX2_MASK_4444_555) > 0;
        
        /* Fetch the texture pixel */
        color = pcx2_texture_pixel(ctx, x, y, a, b, c, d, e, f, p, q, r, exp, 
                                  texture_addr, map_size, pmip, color_depth, 
                                  mip_mapped, col4444or555, global_trans, 
                                  flip_uv, trans_pass);
        
        /* Apply flat shading if not using smooth shading */
        if ((params & PCX2_MASK_SMOOTH_SHADE) == 0) {
            /* Multiply by the base color */
            color.r = (color.r * base.r) >> 8;
            color.g = (color.g * base.g) >> 8;
            color.b = (color.b * base.b) >> 8;
        }
        
        inc_addr += 8;
    } else {
        /* No texture - use base color */
        color.r = base.r;
        color.g = base.g;
        color.b = base.b;
        color.a = 0;
        inc_addr += 2;
    }
    
    /* Apply smooth shading if enabled */
    if (params & PCX2_MASK_SMOOTH_SHADE) {
        /* Extract shading parameters */
        t0 = (int16_t)(ctx->param_memory[inc_addr]);
        t1 = (int16_t)(ctx->param_memory[inc_addr + 1] >> 16);
        t2 = (int16_t)(ctx->param_memory[inc_addr + 1]);
        
        /* Calculate shading intensity */
        fraction = pcx2_linear_shade(t0, t1, t2, x - x_offset, y - y_offset);
        
        /* Extract base color */
        uint16_t base_color_raw = ctx->param_memory[inc_addr] >> 16;
        hold_col.r = ((base_color_raw >> 10) & 0x1F);
        hold_col.g = ((base_color_raw >> 5) & 0x1F);
        hold_col.b = (base_color_raw & 0x1F);
        
        /* Clamp fraction */
        fraction = (fraction < 0) ? 0 : fraction;
        fraction = (fraction > 0x10000) ? 0x10000 : fraction;
        
        /* Reduce resolution to 9 bits */
        fraction >>= 8;
        
        /* Apply shading */
        hold_col.r = (hold_col.r * fraction) >> 5;
        hold_col.g = (hold_col.g * fraction) >> 5;
        hold_col.b = (hold_col.b * fraction) >> 5;
        
        /* Adjust for full intensity */
        if (fraction == 0x100) {
            hold_col.r += 4;
            hold_col.g += 4;
            hold_col.b += 4;
        }
        
        inc_addr += 2;
        
        /* Apply shadow light if enabled */
        if ((params & PCX2_MASK_SHADOW_FLAG) && (shadow == 0)) {
            t0 = (int16_t)(ctx->param_memory[inc_addr]);
            t1 = (int16_t)(ctx->param_memory[inc_addr + 1] >> 16);
            t2 = (int16_t)(ctx->param_memory[inc_addr + 1]);
            
            fraction = pcx2_linear_shade(t0, t1, t2, x - x_offset, y - y_offset);
            
            uint16_t shadow_color_raw = ctx->param_memory[inc_addr] >> 16;
            hold_col1.r = ((shadow_color_raw >> 10) & 0x1F);
            hold_col1.g = ((shadow_color_raw >> 5) & 0x1F);
            hold_col1.b = (shadow_color_raw & 0x1F);
            
            /* Clamp fraction */
            fraction = (fraction < 0) ? 0 : fraction;
            fraction = (fraction > 0x10000) ? 0x10000 : fraction;
            
            /* Reduce resolution to 9 bits */
            fraction >>= 8;
            
            red = (hold_col1.r * fraction) >> 5;
            green = (hold_col1.g * fraction) >> 5;
            blue = (hold_col1.b * fraction) >> 5;
            
            if (fraction == 0x100) {
                red += 4;
                green += 4;
                blue += 4;
            }
            
            /* Add to base color */
            red += hold_col.r;
            green += hold_col.g;
            blue += hold_col.b;
            
            /* Clamp to 255 */
            hold_col.r = (red > 255) ? 255 : red;
            hold_col.g = (green > 255) ? 255 : green;
            hold_col.b = (blue > 255) ? 255 : blue;
            
            inc_addr += 2;
        }
        
        /* Apply to color */
        if ((params & PCX2_MASK_TEXTURE) == 0) {
            color.r = hold_col.r;
            color.g = hold_col.g;
            color.b = hold_col.b;
            color.a = 0;
        } else {
            color.r = (color.r * hold_col.r) >> 8;
            color.g = (color.g * hold_col.g) >> 8;
            color.b = (color.b * hold_col.b) >> 8;
        }
    }
    
    /* Apply flat highlights if enabled */
    if (params & PCX2_MASK_FLAT_HIGHLIGHT) {
        uint16_t highlight_raw = ctx->param_memory[inc_addr] >> 16;
        uint16_t shadow_highlight_raw = ctx->param_memory[inc_addr];
        
        highlight_col.r = ((highlight_raw >> 10) & 0x1F);
        highlight_col.g = ((highlight_raw >> 5) & 0x1F);
        highlight_col.b = (highlight_raw & 0x1F);
        
        shadow_highlight_col.r = ((shadow_highlight_raw >> 10) & 0x1F);
        shadow_highlight_col.g = ((shadow_highlight_raw >> 5) & 0x1F);
        shadow_highlight_col.b = (shadow_highlight_raw & 0x1F);
        
        /* Apply shadow light's highlight if needed */
        if ((params & PCX2_MASK_SHADOW_FLAG) && (shadow == 0)) {
            red = highlight_col.r + shadow_highlight_col.r;
            green = highlight_col.g + shadow_highlight_col.g;
            blue = highlight_col.b + shadow_highlight_col.b;
            
            /* Clamp to 31 */
            highlight_col.r = (red > 31) ? 31 : red;
            highlight_col.g = (green > 31) ? 31 : green;
            highlight_col.b = (blue > 31) ? 31 : blue;
        }
        
        /* Add highlight to color */
        red = color.r + (highlight_col.r << 3);
        green = color.g + (highlight_col.g << 3);
        blue = color.b + (highlight_col.b << 3);
        
        /* Clamp to 255 */
        color.r = (red > 255) ? 255 : red;
        color.g = (green > 255) ? 255 : green;
        color.b = (blue > 255) ? 255 : blue;
        
        inc_addr += 1;
    }
    
    /* Apply fog if enabled */
    if ((params & PCX2_MASK_DISABLE_FOG) == 0 && ctx->fog_enable) {
        /* Interpolate with fog color based on fog amount */
        red = ((ctx->fog_color.r - color.r) * fog) >> 8;
        green = ((ctx->fog_color.g - color.g) * fog) >> 8;
        blue = ((ctx->fog_color.b - color.b) * fog) >> 8;
        
        color.r += red;
        color.g += green;
        color.b += blue;
    }
    
    /* Apply transparency if enabled */
    if ((params & PCX2_MASK_TRANS) > 0) {
        /* Make alpha 15 completely translucent */
        if (color.a == 15)
            color.a = 16;
            
        /* Get current framebuffer color */
        pcx2_rgb_t fb_color = ctx->frame_buffer[y * ctx->frame_width + x];
        
        /* Interpolate between current and new color based on alpha */
        red = (fb_color.r * color.a) >> 4;
        green = (fb_color.g * color.a) >> 4;
        blue = (fb_color.b * color.a) >> 4;
        
        red += ((16 - color.a) * color.r) >> 4;
        green += ((16 - color.a) * color.g) >> 4;
        blue += ((16 - color.a) * color.b) >> 4;
        
        color.r = red;
        color.g = green;
        color.b = blue;
    }
    
    /* Write final color to framebuffer */
    ctx->frame_buffer[y * ctx->frame_width + x].r = color.r;
    ctx->frame_buffer[y * ctx->frame_width + x].g = color.g;
    ctx->frame_buffer[y * ctx->frame_width + x].b = color.b;
    
    /* Update statistics */
    ctx->pixels_rendered++;
}

/*
 * Tile processing function
 * Processes a tile by rendering all pixels within it
 */
static void 
pcx2_process_tile(pcx2_render_context_t *ctx, pcx2_tile_t *tile)
{
    int x, y;
    
    /* Process all pixels in the tile */
    for (y = tile->y; y < tile->y + tile->height; y++) {
        for (x = tile->x; x < tile->x + tile->width; x++) {
            /* Render each pixel with tile parameters */
            pcx2_render_pixel(ctx, x, y, tile->param_addr, 0, 0);
        }
    }
    
    /* Update statistics */
    ctx->tiles_processed++;
}

/*
 * Initialize rendering context
 * Sets up the PCX2 rendering context with necessary memory buffers
 */
static void 
pcx2_init_render_context(pcx2_render_context_t *ctx, uint32_t *texture_mem, uint32_t *param_mem)
{
    /* Initialize basic properties */
    memset(ctx, 0, sizeof(pcx2_render_context_t));
    
    /* Set memory pointers */
    ctx->texture_memory = texture_mem;
    ctx->param_memory = param_mem;
    
    /* Set memory sizes */
    ctx->texture_mem_size = PCX2_TEXTURE_MEMORY_SIZE;
    ctx->param_mem_size = 1024 * 1024; /* 1MB parameter memory */
    
    /* Set default rendering state */
    ctx->render_state = PCX2_RENDER_STATE_IDLE;
    
    /* Set default settings */
    ctx->camera_scale = 0x4000;
    ctx->texture_filter = PCX2_FILTER_POINT_SAMPLE;
    ctx->dithering = 0;
    ctx->fog_enable = 1;
    
    /* Set default fog color */
    ctx->fog_color.r = 128;
    ctx->fog_color.g = 128;
    ctx->fog_color.b = 128;
    
    /* Initialize statistics */
    ctx->frame_count = 0;
    ctx->pixels_rendered = 0;
    ctx->tiles_processed = 0;
    ctx->texture_cache_hits = 0;
    ctx->texture_cache_misses = 0;
    
    /* Initialize texture cache */
    for (int i = 0; i < 16; i++) {
        ctx->texture_cache[i].valid = 0;
        ctx->texture_cache[i].dirty = 0;
        ctx->texture_cache[i].lru = 0;
    }
}

/*
 * Reset rendering context
 * Resets the PCX2 rendering context for a new rendering operation
 */
static void 
pcx2_reset_render_context(pcx2_render_context_t *ctx)
{
    /* Reset rendering state */
    ctx->render_state = PCX2_RENDER_STATE_IDLE;
    
    /* Reset per-frame statistics */
    ctx->pixels_rendered = 0;
    ctx->tiles_processed = 0;
    ctx->texture_cache_hits = 0;
    ctx->texture_cache_misses = 0;
}

/*
 * Start frame rendering
 * Begins a new frame rendering operation
 */
static void 
pcx2_start_frame(pcx2_render_context_t *ctx)
{
    /* Update rendering state */
    ctx->render_state = PCX2_RENDER_STATE_ACTIVE;
    
    /* Increment frame counter */
    ctx->frame_count++;
}

/*
 * End frame rendering
 * Completes the current frame rendering operation
 */
static void 
pcx2_end_frame(pcx2_render_context_t *ctx)
{
    /* Update rendering state */
    ctx->render_state = PCX2_RENDER_STATE_IDLE;
}

/* Exported functions for PCX2 core integration */

/*
 * PCX2 Initialize Render Pipeline
 * Initializes the 3D rendering pipeline
 */
void 
pcx2_init_render_pipeline(void *pcx2_device, uint32_t *texture_mem, uint32_t *param_mem)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)malloc(sizeof(pcx2_render_context_t));
    if (!ctx) {
        pclog("PCX2 Render: Failed to allocate rendering context\n");
        return;
    }
    
    /* Initialize the rendering context */
    pcx2_init_render_context(ctx, texture_mem, param_mem);
    
    /* Store render context in PCX2 device structure */
    /* This would be integrated with the PCX2 core */
}

/*
 * PCX2 Shutdown Render Pipeline
 * Shuts down the 3D rendering pipeline and frees resources
 */
void 
pcx2_shutdown_render_pipeline(void *render_context)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        /* Free the rendering context */
        free(ctx);
    }
}

/*
 * PCX2 Render Frame
 * Renders a frame based on the current parameter data
 */
void pcx2_render_frame(void *render_context, pcx2_rgb_t *frame_buffer, int width, int height, uint32_t *object_list, int object_count)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (!ctx) {
        pclog("PCX2 Render: Invalid rendering context\n");
        return;
    }
    
    /* Set frame buffer information */
    ctx->frame_buffer = frame_buffer;
    ctx->frame_width = width;
    ctx->frame_height = height;
    
    /* Start frame rendering */
    pcx2_start_frame(ctx);
    
    /* Process all objects */
    for (int i = 0; i < object_count; i++) {
        /* Create a tile for the object */
        pcx2_tile_t tile;
        
        /* Extract tile information from object */
        tile.x = (object_list[i * 4 + 0] >> 0) & 0xFFFF;
        tile.y = (object_list[i * 4 + 0] >> 16) & 0xFFFF;
        tile.width = (object_list[i * 4 + 1] >> 0) & 0xFF;
        tile.height = (object_list[i * 4 + 1] >> 8) & 0xFF;
        tile.param_addr = object_list[i * 4 + 2];
        
        /* Process the tile */
        pcx2_process_tile(ctx, &tile);
    }
    
    /* End frame rendering */
    pcx2_end_frame(ctx);
}

/*
 * PCX2 Set Camera Scale
 * Sets the camera scale factor for texture mapping
 */
void 
pcx2_set_render_camera_scale(void *render_context, uint16_t scale)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        ctx->camera_scale = scale;
    }
}

/*
 * PCX2 Set Fog Color
 * Sets the fog color for atmospheric effects
 */
void 
pcx2_set_render_fog_color(void *render_context, uint8_t r, uint8_t g, uint8_t b)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        ctx->fog_color.r = r;
        ctx->fog_color.g = g;
        ctx->fog_color.b = b;
    }
}

/*
 * PCX2 Set Texture Filtering Mode
 * Sets the texture filtering mode
 */
void 
pcx2_set_render_texture_filtering(void *render_context, uint8_t mode)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        ctx->texture_filter = mode & 0x03;
    }
}

/*
 * PCX2 Set Fog Enable
 * Enables or disables fog effects
 */
void 
pcx2_set_render_fog_enable(void *render_context, uint8_t enable)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        ctx->fog_enable = enable ? 1 : 0;
    }
}

/*
 * PCX2 Set Dithering
 * Enables or disables color dithering
 */
void 
pcx2_set_render_dithering(void *render_context, uint8_t enable)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        ctx->dithering = enable ? 1 : 0;
    }
}

/*
 * PCX2 Get Render Stats
 * Retrieves rendering statistics
 */
void 
pcx2_get_render_stats(void *render_context, uint32_t *frames, uint32_t *pixels, 
                     uint32_t *tiles, uint32_t *cache_hits, uint32_t *cache_misses)
{
    pcx2_render_context_t *ctx = (pcx2_render_context_t *)render_context;
    if (ctx) {
        if (frames) *frames = ctx->frame_count;
        if (pixels) *pixels = ctx->pixels_rendered;
        if (tiles) *tiles = ctx->tiles_processed;
        if (cache_hits) *cache_hits = ctx->texture_cache_hits;
        if (cache_misses) *cache_misses = ctx->texture_cache_misses;
    }
}