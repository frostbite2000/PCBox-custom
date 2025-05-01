/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          The insane Rage128 MMIO/Register arbiter.
 *
 *          
 *
 * Authors: frostbite2000
 *
 *          Copyright 2025 frostbite2000
 */

// STANDARD Rage128 includes
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <86Box/86box.h>
#include <86Box/device.h>
#include <86Box/mem.h>
#include <86box/pci.h>
#include <86Box/rom.h> // DEPENDENT!!!
#include <86Box/video.h>
#include <86Box/ati/vid_ati.h>
#include <86Box/ati/vid_r128.h>

// Arbitrates an MMIO read
uint32_t r128_mmio_arbitrate_read(uint32_t address)
{
    // sanity check
    if (!r128)
        return 0x00; 

    uint32_t ret = 0x00;
    
    if (!(address >= R128_DAC_MASK && address <= R128_DAC_DATA))
        address &= 0x4000;

    switch (address) {
        case R128_CRTC_H_TOTAL_DISP:
            ret = r128->crtc_h_total_disp;
            break;
        case R128_CRTC_V_TOTAL_DISP:
            ret = r128->crtc_v_total_disp;
            break;
        case R128_CRTC_GEN_CNTL:
            ret = r128->crtc_gen_cntl;
            break;
        case R128_CRTC_EXT_CNTL:
            ret = r128->crtc_ext_cntl;
            break;
        case R128_CONFIG_MEMSIZE:
            ret = r128->config_memsize;
            break;
    }

    ati_log("R128: MMIO read, 0x%08x <- 0x%08x\n", ret, address);
    return ret;
}

void r128_mmio_arbitrate_write(uint32_t address, uint32_t value)
{
    // sanity check
    if (!r128)
        return; 

    r128_t* r128 = (r128_t*)svga->priv;

    if (!(address >= R128_DAC_MASK && address <= R128_DAC_DATA))
        address &= 0x4000;

    switch (address) {
        case R128_CRTC_H_TOTAL_DISP:
            r128->crtc_h_total_disp &= ~value;
            break;
        case R128_CRTC_V_TOTAL_DISP:
            r128->crtc_v_total_disp &= ~value;
            break;
        case R128_CRTC_GEN_CNTL:
            r128->crtc_gen_cntl &= ~value;
            r128_recalc_timings(&r128->atibase.svga);
            break;
        case R128_CRTC_EXT_CNTL:
            r128->crtc_ext_cntl &= ~value;
            r128_recalc_timings(&r128->atibase.svga);
            break;
        case R128_CONFIG_MEMSIZE:
            r128->config_memsize &= (r128->atibase.vram_amount);
            break;
    }

    ati_log("R128: MMIO write, 0x%08x -> 0x%08x\n", value, address);
}
