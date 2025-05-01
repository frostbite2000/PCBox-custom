/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Provides Rage128 configuration
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
#include <86box/86box.h>
#include <86box/device.h>
#include <86box/mem.h>
#include <86box/io.h>
#include <86box/pci.h>
#include <86box/rom.h> // DEPENDENT!!!
#include <86box/video.h>
#include <86box/ati/vid_ati.h>
#include <86box/ati/vid_r128.h>

const device_config_t r128_config[] =
{
    // VBIOS type configuration
    {
        .name = "vbios",
        .description = "Model",
        .type = CONFIG_BIOS,
        .default_string = "R128_VBIOS_RAGE128PRO_16MB",
        .default_int = 0,
        .bios = 
        {

           { 
                .name = "Rage 128 GL 16 MB", .files_no = 1,
                .internal_name = "R128_VBIOS_RAGE128PRO_16MB",
                .files = {R128_VBIOS_RAGE128PRO_16MB, ""} 
           },
           { 
                .name = "Rage 128 Pro 32 MB", .files_no = 1,
                .internal_name = "R128_VBIOS_RAGE128PRO_32MB",
                .files = {R128_VBIOS_RAGE128PRO_32MB, ""} 
           },
        }
    },
    // Memory configuration
    {
        .name = "vram_size",
        .description = "VRAM Size",
        .type = CONFIG_SELECTION,
        .default_int = R128_VRAM_SIZE_16MB,
        .selection = 
        {
            {
                .description = "16 MB",
                .value = R128_VRAM_SIZE_16MB,
            },

            {
                .description = "32 MB",
                .value = R128_VRAM_SIZE_32MB,
            },
        }

    },
    {
        .type = CONFIG_END
    }
};