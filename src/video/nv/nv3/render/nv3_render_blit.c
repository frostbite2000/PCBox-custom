/*
* 86Box    A hypervisor and IBM PC system emulator that specializes in
*          running old operating systems and software designed for IBM
*          PC systems and compatibles from 1981 through fairly recent
*          system designs based on the PCI bus.
*
*          This file is part of the 86Box distribution.
*
*          NV3 Core rendering code (Software version)
*
* 
* 
* Authors: Connor Hyde, <mario64crashed@gmail.com> I need a better email address ;^)
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
#include <86box/plat.h>
#include <86box/rom.h>
#include <86box/video.h>
#include <86box/nv/vid_nv.h>
#include <86box/nv/vid_nv3.h>

/* Check the line bounds */
void nv3_class_011_check_line_bounds(void)
{                
    uint32_t relative_x = nv3->pgraph.image_current_position.x - nv3->pgraph.image.point.x;

    /* In the case of class 0x11 there is no requirement to check for relative_y because we have exceeded the size of the image */
    if (relative_x >= nv3->pgraph.image.size_in.x)
    {   
        nv3->pgraph.image_current_position.y++;
        nv3->pgraph.image_current_position.x = nv3->pgraph.image.point.x;
    }
}

/* Renders an image from cpu */
void nv3_render_blit_image(uint32_t color, nv3_grobj_t grobj)
{
    /* todo: a lot of stuff */

    uint32_t pixel0 = 0, pixel1 = 0, pixel2 = 0, pixel3 = 0;

    /* Some extra data is sent as padding, we need to clip it off using size_out */

    uint16_t clip_x = nv3->pgraph.image.point.x + nv3->pgraph.image.size.x;
    /* we need to unpack them - IF THIS IS USED SOMEWHERE ELSE, DO SOMETHING ELSE WITH IT */
    /* the reverse order is due to the endianness */
    switch (nv3->nvbase.svga.bpp)
    {
        // 4pixels packed into one color
        case 8:
        
            //pixel3
            pixel3 = color & 0xFF;
            if (nv3->pgraph.image_current_position.x < clip_x) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel3, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();

            pixel2 = (color >> 8) & 0xFF;
            if (nv3->pgraph.image_current_position.x < clip_x) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel2, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();
            
            pixel1 = (color >> 16) & 0xFF;
            if (nv3->pgraph.image_current_position.x < clip_x) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel1, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();

            pixel0 = (color >> 24) & 0xFF;
            if (nv3->pgraph.image_current_position.x < clip_x) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel0, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();

            break;
        //2pixels packed into one color
        case 15:
        case 16:
            pixel1 = (color) & 0xFFFF;
            if (nv3->pgraph.image_current_position.x < (clip_x)) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel1, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();

            pixel0 = (color >> 16) & 0xFFFF;
            if (nv3->pgraph.image_current_position.x < (clip_x)) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel0, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();
                
            break;
        // just one pixel in 32bpp
        case 32: 
            pixel0 = color;
            if (nv3->pgraph.image_current_position.x < clip_x) nv3_render_write_pixel(nv3->pgraph.image_current_position, pixel0, grobj);
            nv3->pgraph.image_current_position.x++;
            nv3_class_011_check_line_bounds();

            break;
    }
}


#define NV3_MAX_HORIZONTAL_SIZE     1920
#define NV3_MAX_VERTICAL_SIZE       1200

/* 1920 for margin. Holds a buffer of the old screen we want to hold so we don't overwrite things we already overwtote
We only need to clear it once per blit, because the blits are always the same size, and then only for the size of our new blit 

Extremely not crazy about this...Surely a better way to do it without buffering the ENTIRE SCREEN. I only update the parts that are needed, but still...

This is LUDICROUSLY INEFFICIENT (2*O(n^2)) and COMPLETELY TERRIBLE code, but it's currently 2:48am so I can't think of a better approach... 
*/
uint32_t nv3_s2sb_line_buffer[NV3_MAX_HORIZONTAL_SIZE*NV3_MAX_VERTICAL_SIZE] = {0};

void nv3_render_blit_screen2screen(nv3_grobj_t grobj)
{
    if (nv3->pgraph.blit.size.x < NV3_MAX_HORIZONTAL_SIZE
    && nv3->pgraph.blit.size.y < NV3_MAX_VERTICAL_SIZE)
        memset(&nv3_s2sb_line_buffer, 0x00, (sizeof(uint32_t) * nv3->pgraph.blit.size.y) * (sizeof(uint32_t) * nv3->pgraph.blit.size.x));

    /* First calculate our source and destination buffer */
    uint32_t src_buffer = (grobj.grobj_0 >> NV3_PGRAPH_CONTEXT_SWITCH_SRC_BUFFER) & 0x03;
    uint32_t dst_buffer = 0; // 5 = just use the source buffer

    if ((grobj.grobj_0 >> NV3_PGRAPH_CONTEXT_SWITCH_DST_BUFFER0_ENABLED) & 0x01) dst_buffer = 0;
    if ((grobj.grobj_0 >> NV3_PGRAPH_CONTEXT_SWITCH_DST_BUFFER1_ENABLED) & 0x01) dst_buffer = 1;
    if ((grobj.grobj_0 >> NV3_PGRAPH_CONTEXT_SWITCH_DST_BUFFER2_ENABLED) & 0x01) dst_buffer = 2;
    if ((grobj.grobj_0 >> NV3_PGRAPH_CONTEXT_SWITCH_DST_BUFFER3_ENABLED) & 0x01) dst_buffer = 3;
    
    bool cross_buffer_blit = (nv3->pgraph.boffset[src_buffer] != nv3->pgraph.boffset[dst_buffer]);

    nv3_coord_16_t old_position = nv3->pgraph.blit.point_in;
    nv3_coord_16_t new_position = nv3->pgraph.blit.point_out;

    /* Coordinates for copying an entire line at a time */
    uint32_t buf_position = 0, vram_position = 0, size_x = nv3->pgraph.blit.size.x;

    /* Read the old pixel into the line buffer
       Assumption: All data is sent in an unpacked format. In the case of an NVIDIA GPU this means that all data is sent 32 bits at a time regardless of if
       the actual source data is 32 bits in size or not. For pixel data, the upper bits are left as 0 in 8bpp/16bpp mode. For 86box purposes, the data is written
       8/16 bits at a time.

       TODO: CHECK FOR PACKED FORMAT!!!!!
    */

    if (nv3->nvbase.svga.bpp == 15
    || nv3->nvbase.svga.bpp == 16)
        size_x <<= 1;
    else if (nv3->nvbase.svga.bpp == 32)
        size_x <<= 2;
        
    for (int32_t y = 0; y < nv3->pgraph.blit.size.y; y++)
    {
        buf_position = (nv3->pgraph.blit.size.x * y);
        /* shouldn't matter in non-wtf mode */
        vram_position = nv3_render_get_vram_address_for_buffer(old_position, src_buffer);

        memcpy(&nv3_s2sb_line_buffer[buf_position], &nv3->nvbase.svga.vram[vram_position], size_x);
        old_position.y++;
        /* 32bit buffer */
    }
    
    /* simply write it all back to vram */
    for (int32_t y = 0; y < nv3->pgraph.blit.size.y; y++)
    {        
        buf_position = (nv3->pgraph.blit.size.x * y);
        vram_position = nv3_render_get_vram_address_for_buffer(new_position, dst_buffer);

        memcpy(&nv3->nvbase.svga.vram[vram_position], &nv3_s2sb_line_buffer[buf_position], size_x);
        new_position.y++;
    }

    /* 
        We need to blit manually here because we don't go through nv3_render_write_pixel
        We also need to update all of the areas of the screen that moved. 
    */

    nv3_coord_16_t blit_position = {0};
    nv3_coord_16_t blit_size = {0};

    /* Change the smallest area of the screen that moved */

    if (cross_buffer_blit)
    {
        blit_position = nv3->pgraph.blit.point_out;
        blit_size = nv3->pgraph.blit.size;
    }
    else
    {
        if (nv3->pgraph.blit.point_out.x > nv3->pgraph.blit.point_in.x)
            blit_size.x = (nv3->pgraph.blit.point_out.x - nv3->pgraph.blit.point_in.x) + nv3->pgraph.blit.size.x;
        else if (nv3->pgraph.blit.point_out.x < nv3->pgraph.blit.point_in.x)
            blit_size.x = (nv3->pgraph.blit.point_in.x - nv3->pgraph.blit.point_out.x) + nv3->pgraph.blit.size.x;
        else
            blit_size.x = nv3->pgraph.blit.size.x;

        if (nv3->pgraph.blit.point_out.y > nv3->pgraph.blit.point_in.y)
            blit_size.y = (nv3->pgraph.blit.point_out.y - nv3->pgraph.blit.point_in.y) + nv3->pgraph.blit.size.y;
        else if (nv3->pgraph.blit.point_out.y < nv3->pgraph.blit.point_in.y)
            blit_size.y = (nv3->pgraph.blit.point_in.y - nv3->pgraph.blit.point_out.y) + nv3->pgraph.blit.size.y;
        else
            blit_size.y = nv3->pgraph.blit.size.y; 

        if (nv3->pgraph.blit.point_out.x > nv3->pgraph.blit.point_in.x)
            blit_position.x = nv3->pgraph.blit.point_in.x;
        else if (nv3->pgraph.blit.point_out.x <= nv3->pgraph.blit.point_in.x) // equals case, just use out 
            blit_position.x = nv3->pgraph.blit.point_out.x;

        if (nv3->pgraph.blit.point_out.y > nv3->pgraph.blit.point_in.y)
            blit_position.y = nv3->pgraph.blit.point_in.y;
        else if (nv3->pgraph.blit.point_out.y <= nv3->pgraph.blit.point_in.y) // equals case, just use out 
            blit_position.y = nv3->pgraph.blit.point_out.y;

    }

    /* If the BUFFER_ADDRESS of the last buffer is not the DBA, we don't *actually* want to draw this, so let's not
    Apply stupid hack */
    

    nv3_render_current_bpp(&nv3->nvbase.svga, blit_position, blit_size, grobj, false, true);
}