/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Rage128 bringup and device emulation.
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2024-2025 frostbite2000
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

/* Main device object pointer */
r128_t* r128;

/* These are a ****PLACEHOLDER**** and are copied from 3dfx VoodooBanshee/Voodoo3*/
static video_timings_t timing_r128_agp = { .type = VIDEO_AGP, .write_b = 2, .write_w = 2, .write_l = 1, .read_b = 20, .read_w = 20, .read_l = 21 };

// Prototypes for functions only used in this translation unit
void r128_init_mappings_mmio(void);
void r128_init_mappings_svga(void);
bool r128_is_svga_redirect_address(uint32_t addr);

uint8_t r128_svga_read(uint16_t addr, void* priv);
void r128_svga_write(uint16_t addr, uint8_t val, void* priv);

// Determine if this address needs to be redirected to the SVGA subsystem.

bool r128_is_svga_redirect_address(uint32_t addr)
{
    return (addr >= R128_DAC_MASK && addr <= R128_DAC_DATA);
}

// All MMIO regs are 32-bit i believe internally
// so we have to do some munging to get this to read

// Read 8-bit MMIO
uint8_t r128_mmio_read8(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    // We need to specifically exclude this particular set of registers
    // so we can write the 4/8bpp CLUT
    if (addr >= R128_DAC_MASK && addr <= R128_DAC_DATA) 
    {
        // Throw directly into PRAMDAC
        return r128_mmio_arbitrate_read(addr);
    }
        
    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = r128_svga_read(real_address, r128);

        ati_log_verbose_only("Redirected MMIO read8 to SVGA: addr=0x%04x returned 0x%04x\n", addr, ret);

        return ret; 
    }

    // see if unaligned reads are a problem
    ret = r128_mmio_read32(addr, priv);
    return (uint8_t)(ret >> ((addr & 3) << 3) & 0xFF);
}

// Read 16-bit MMIO
uint16_t r128_mmio_read16(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = r128_svga_read(real_address, r128)
        | (r128_svga_read(real_address + 1, r128) << 8);
        
        ati_log_verbose_only("Redirected MMIO read16 to SVGA: addr=0x%04x returned 0x%04x\n", addr, ret);

        return ret; 
    }

    ret = r128_mmio_read32(addr, priv);
    return (uint8_t)(ret >> ((addr & 3) << 3) & 0xFFFF);
}

// Read 32-bit MMIO
uint32_t r128_mmio_read32(uint32_t addr, void* priv)
{
    uint32_t ret = 0x00;

    // Some of these addresses are Weitek VGA stuff and we need to mask it to this first because the weitek addresses are 8-bit aligned.
    addr &= 0xFFFFFF;

    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ret = r128_svga_read(real_address, r128)
        | (r128_svga_read(real_address + 1, r128) << 8)
        | (r128_svga_read(real_address + 2, r128) << 16)
        | (r128_svga_read(real_address + 3, r128) << 24);

        ati_log_verbose_only("Redirected MMIO read32 to SVGA: addr=0x%04x returned 0x%04x\n", addr, ret);

        return ret; 
    }


    ret = r128_mmio_arbitrate_read(addr);


    return ret; 

}

// Write 8-bit MMIO
void r128_mmio_write8(uint32_t addr, uint8_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // We need to specifically exclude this particular set of registers
    // so we can write the 4/8bpp CLUT
    if (addr >= R128_DAC_MASK && addr <= R128_DAC_DATA) 
    {
        // Throw directly into PRAMDAC
        r128_mmio_arbitrate_write(addr, val);
        return; 
    }

    // This is weitek vga stuff
    // If we need to add more of these we can convert these to a switch statement
    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ati_log_verbose_only("Redirected MMIO write8 to SVGA: addr=0x%04x val=0x%02x\n", addr, val);

        r128_svga_write(real_address, val & 0xFF, r128);

        return; 
    }
    
    // overwrite first 8bits of a 32 bit value
    uint32_t new_val = r128_mmio_read32(addr, NULL);

    new_val &= (~0xFF << (addr & 3) << 3);
    new_val |= (val << ((addr & 3) << 3));

    r128_mmio_write32(addr, new_val, priv);
}

// Write 16-bit MMIO
void r128_mmio_write16(uint32_t addr, uint16_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // This is weitek vga stuff
    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ati_log_verbose_only("Redirected MMIO write16 to SVGA: addr=0x%04x val=0x%02x\n", addr, val);

        r128_svga_write(real_address, val & 0xFF, r128);
        r128_svga_write(real_address + 1, (val >> 8) & 0xFF, r128);
        
        return; 
    }

    // overwrite first 16bits of a 32 bit value
    uint32_t new_val = r128_mmio_read32(addr, NULL);

    new_val &= (~0xFFFF << (addr & 3) << 3);
    new_val |= (val << ((addr & 3) << 3));

    r128_mmio_write32(addr, new_val, priv);
}

// Write 32-bit MMIO
void r128_mmio_write32(uint32_t addr, uint32_t val, void* priv)
{
    addr &= 0xFFFFFF;

    // This is weitek vga stuff
    if (r128_is_svga_redirect_address(addr))
    {
        // svga writes are not logged anyway rn
        uint32_t real_address = addr & 0x3FF;

        ati_log_verbose_only("Redirected MMIO write32 to SVGA: addr=0x%04x val=0x%02x\n", addr, val);

        r128_svga_write(real_address, val & 0xFF, r128);
        r128_svga_write(real_address + 1, (val >> 8) & 0xFF, r128);
        r128_svga_write(real_address + 2, (val >> 16) & 0xFF, r128);
        r128_svga_write(real_address + 3, (val >> 24) & 0xFF, r128);
        
        return; 
    }

    r128_mmio_arbitrate_write(addr, val);
}

// PCI stuff
// BAR0         Pointer to MMIO space
// BAR1         Pointer to Linear Framebuffer (NV_USER)

uint8_t r128_pci_read(int32_t func, int32_t addr, void* priv)
{
    uint8_t ret = 0x00;

    // sanity check
    if (!r128)
        return ret; 

    // figure out what size this gets read as first
    // seems func does not matter at least here?
    switch (addr) 
    {
        // Get the pci vendor id..

        case R128_PCI_CFG_VENDOR_ID:
            ret = (PCI_VENDOR_ATI & 0xFF);
            break;
        
        case R128_PCI_CFG_VENDOR_ID + 1: // all access 8bit
            ret = (PCI_VENDOR_ATI >> 8);
            break;

        // device id

        case R128_PCI_CFG_DEVICE_ID:
            ret = (ATI_PCI_DEVICE_R128 & 0xFF);
            break;
        
        case R128_PCI_CFG_DEVICE_ID + 1:
            ret = (ATI_PCI_DEVICE_R128 >> 8);
            break;

        case PCI_REG_COMMAND_L:
            ret = r128->pci_config.pci_regs[PCI_REG_COMMAND_L] & (R128_PCI_COMMAND_L_IO || R128_PCI_COMMAND_L_MEMORY || R128_PCI_COMMAND_L_MASTER || R128_PCI_COMMAND_L_SPECIAL); 
            break;
        
        case PCI_REG_COMMAND_H:
            ret = r128->pci_config.pci_regs[PCI_REG_COMMAND_H];
            break;

        // pci status register
        case PCI_REG_STATUS_L:
            ret = r128->pci_config.pci_regs[PCI_REG_STATUS_L] & (R128_PCI_STATUS_L_CAP_LIST || R128_PCI_STATUS_L_FAST_BACK || R128_PCI_STATUS_L_66MHZ_CAPABLE);
            break;

        case PCI_REG_STATUS_H:
            ret = (r128->pci_config.pci_regs[PCI_REG_STATUS_H]) & R128_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING;
            break;
        
        case R128_PCI_CFG_REVISION:
            ret = 0x00; // nothing
            break;
       
        case PCI_REG_PROG_IF:
            ret = 0x00;
            break;
            
        case R128_PCI_CFG_SUBCLASS_CODE:
            ret = 0x00; // nothing
            break;
        
        case R128_PCI_CFG_CLASS_CODE:
            ret = R128_PCI_CFG_CLASS_CODE_VGA; // CLASS_CODE_VGA 
            break;
        
        case R128_PCI_CFG_CACHE_LINE_SIZE:
            ret = R128_PCI_CFG_CACHE_LINE_SIZE_DEFAULT_FROM_VBIOS;
            break;
        
        case R128_PCI_CFG_LATENCY_TIMER:
            ret = 0x40;
            break;

        case R128_PCI_CFG_HEADER_TYPE:
            ret = 0x00;
            break;

        case R128_PCI_CFG_BIST:
            ret = 0x00;
            break;

        case R128_PCI_CFG_BAR0_L:
            ret = 0x08;
            break;

        case R128_PCI_CFG_BAR1_L:
            ret = 0x01;
            break;

        case R128_PCI_CFG_BAR0_BYTE1 ... R128_PCI_CFG_BAR0_BYTE2:
        case R128_PCI_CFG_BAR1_BYTE1 ... R128_PCI_CFG_BAR1_BYTE2:
        case R128_PCI_CFG_BAR2_BYTE1 ... R128_PCI_CFG_BAR2_BYTE2:
            ret = 0x00;
            break;

        // MMIO base address
        case R128_PCI_CFG_BAR0_BASE_ADDRESS:
            ret = r128->atibase.bar0_lfb_base >> 24; //8bit value
            break; 

        case R128_PCI_CFG_BAR1_BASE_ADDRESS:
            ret = r128->atibase.bar1_io_base >> 24; //8bit value
            break;

        case R128_PCI_CFG_BAR2_BASE_ADDRESS:
            ret = r128->atibase.bar2_mmio_base >> 24; //8bit value
            break;

        case R128_PCI_CFG_ENABLE_VBIOS:
            ret = r128->pci_config.vbios_enabled;
            break;
        
        case R128_PCI_CFG_INT_LINE:
            ret = r128->pci_config.int_line;
            break;
        
        case R128_PCI_CFG_INT_PIN:
            ret = PCI_INTA;
            break;

        case R128_PCI_CFG_MIN_GRANT:
            ret = 0x08;
            break;

        case R128_PCI_CFG_MAX_LATENCY:
            ret = 0x00;
            break;

        default: // by default just return pci_config.pci_regs
            ret = r128->pci_config.pci_regs[addr];
            break;
        
    }

    ati_log("r128_pci_read func=0x%04x addr=0x%04x ret=0x%04x\n", func, addr, ret);
    return ret; 
}

uint8_t r128_ext_in(uint16_t addr, void *priv)
{
    // sanity check
    if (!r128)
        return 0x00; 
}

void r128_ext_out(uint16_t addr, uint8_t val, void *priv)
{
    // sanity check
    if (!r128)
        return; 
}

void r128_pci_write(int32_t func, int32_t addr, uint8_t val, void* priv)
{

    // sanity check
    if (!r128)
        return; 

    // some addresses are not writable so can't have any effect and can't be allowed to be modified using this code
    // as an example, only the most significant byte of the PCI BARs can be modified
    if (addr >= R128_PCI_CFG_BAR0_L && addr <= R128_PCI_CFG_BAR0_BYTE2
    && addr >= R128_PCI_CFG_BAR1_L && addr <= R128_PCI_CFG_BAR1_BYTE2)
        return;

    ati_log("r128_pci_write func=0x%04x addr=0x%04x val=0x%04x\n", func, addr, val);

    r128->pci_config.pci_regs[addr] = val;

    switch (addr)
    {
        // standard pci command stuff
        case PCI_REG_COMMAND_L:
            r128->pci_config.pci_regs[PCI_REG_COMMAND_L] = val;
            // actually update the mappings
            r128_update_mappings();
            break;
        case PCI_REG_COMMAND_H:
            r128->pci_config.pci_regs[PCI_REG_COMMAND_H] = val;
            // actually update the mappings
            r128_update_mappings();          
            break;
        // pci status register
        case PCI_REG_STATUS_L:
            r128->pci_config.pci_regs[PCI_REG_STATUS_L] = val | (R128_PCI_STATUS_L_66MHZ_CAPABLE);
            break;
        case PCI_REG_STATUS_H:
            r128->pci_config.pci_regs[PCI_REG_STATUS_H] = val | (R128_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING);
            break;
        //TODO: ACTUALLY REMAP THE MMIO AND NV_USER
        case R128_PCI_CFG_BAR0_BASE_ADDRESS:
            r128->atibase.bar0_lfb_base = val << 24;
            r128_update_mappings();
            break; 
        case R128_PCI_CFG_BAR1_BASE_ADDRESS:
            io_removehandler(r128->atibase.bar1_io_base & 0xff00, 256, r128_ext_in, NULL, NULL, r128_ext_out, NULL, NULL, r128);
            r128->atibase.bar1_io_base = (r128->atibase.bar1_io_base & ~0xff000000) | (val << 24);
            if (r128->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_IO)
                io_sethandler(r128->atibase.bar1_io_base, 256, r128_ext_in, NULL, NULL, r128_ext_out, NULL, NULL, r128);
            break;
        case R128_PCI_CFG_BAR2_BASE_ADDRESS:
            r128->atibase.bar2_mmio_base = val << 24;
            r128_update_mappings();
            break;
        case R128_PCI_CFG_ENABLE_VBIOS:
        case R128_PCI_CFG_VBIOS_BASE:
            
            // make sure we are actually toggling the vbios, not the rom base
            if (addr == R128_PCI_CFG_ENABLE_VBIOS)
                r128->pci_config.vbios_enabled = (val & 0x01);

            if (r128->pci_config.vbios_enabled)
            {
                // First see if we simply wanted to change the VBIOS location

                // Enable it in case it was disabled before
                mem_mapping_enable(&r128->atibase.vbios.mapping);

                if (addr != R128_PCI_CFG_ENABLE_VBIOS)
                {
                    uint32_t old_addr = r128->atibase.vbios.mapping.base;
                    // 9bit register
                    uint32_t new_addr = r128->pci_config.pci_regs[R128_PCI_CFG_VBIOS_BASE_H] << 24 |
                    r128->pci_config.pci_regs[R128_PCI_CFG_VBIOS_BASE_L] << 16;

                    // move it
                    mem_mapping_set_addr(&r128->atibase.vbios.mapping, new_addr, 0x10000);

                    ati_log("...i like to move it move it (VBIOS Relocation) 0x%04x -> 0x%04x\n", old_addr, new_addr);

                }
                else
                {
                    ati_log("...VBIOS Enable\n");
                }
            }
            else
            {
                ati_log("...VBIOS Disable\n");
                mem_mapping_disable(&r128->atibase.vbios.mapping);

            }
            break;
        case R128_PCI_CFG_INT_LINE:
            r128->pci_config.int_line = val;
            break;

        default:
            break;
    }
}


//
// SVGA functions
//
void r128_recalc_timings(svga_t* svga)
{    
    // sanity check
    if (!r128)
        return; 

    r128_t* r128 = (r128_t*)svga->priv;
    uint32_t pixel_mode = svga->crtc[R128_CRTC_GEN_CNTL] & R128_CRTC_PIX_WIDTH;
    uint32_t crtc_gen_cntl = svga->crtc[R128_CRTC_GEN_CNTL] & R128_CRTC_EXT_DISP_EN;

    svga->override = !(pixel_mode == R128_CRTC_VGA_XOVERSCAN);

    if (crtc_gen_cntl) {
        switch (pixel_mode)
        {
            case R128_CRTC_PIX_WIDTH_PIXELMODE_4BPP:
                svga->bpp = 4;
                svga->lowres = 0;
                svga->map8 = svga->pallook;
                break;
            case R128_CRTC_PIX_WIDTH_PIXELMODE_8BPP:
                svga->bpp = 8;
                svga->lowres = 0;
                svga->map8 = svga->pallook;
                break;
            case R128_CRTC_PIX_WIDTH_PIXELMODE_15BPP:
                svga->bpp = 15;
                svga->lowres = 0;
                break;
            case R128_CRTC_PIX_WIDTH_PIXELMODE_16BPP:
                svga->bpp = 16;
                svga->lowres = 0;
                break;
            case R128_CRTC_PIX_WIDTH_PIXELMODE_24BPP:
                svga->bpp = 24;
                svga->lowres = 0;
                break;
            case R128_CRTC_PIX_WIDTH_PIXELMODE_32BPP:
                svga->bpp = 32;
                svga->lowres = 0;
                break;
        }
    }
}

void r128_speed_changed(void* priv)
{
    // sanity check
    if (!r128)
        return; 
        
    r128_recalc_timings(&r128->atibase.svga);
}

// Force Redraw
// Reset etc.
void r128_force_redraw(void* priv)
{
    // sanity check
    if (!r128)
        return; 

    r128->atibase.svga.fullchange = changeframecount; 
}

void
r128_svga_write(uint16_t addr, uint8_t val, void *priv)
{
    // sanity check
    if (!r128)
        return; 

    // mask off b0/d0 registers 
    if ((((addr & 0xFFF0) == 0x3D0 || (addr & 0xFFF0) == 0x3B0) 
    && addr < 0x3de) 
    && !(r128->atibase.svga.miscout & 1))//miscout bit 7 controls mappping
        addr ^= 0x60;

    uint8_t crtcreg = r128->atibase.svga.crtcreg;
    uint8_t old = 0x00;

    // mask off b0/d0 registers 
    if ((((addr & 0xFFF0) == 0x3D0 
    || (addr & 0xFFF0) == 0x3B0) && addr < 0x3de) 
    && !(r128->atibase.svga.miscout & 1))
        addr ^= 0x60;

    switch (addr) {
        case 0x3D4:
            r128->atibase.svga.crtcreg = val & 0x3f;
            return;
        case 0x3D5:
            if (r128->atibase.svga.crtcreg & 0x20)
                return;
            if ((r128->atibase.svga.crtcreg < 7) && (r128->atibase.svga.crtc[0x11] & 0x80))
                return;
            if ((r128->atibase.svga.crtcreg == 7) && (r128->atibase.svga.crtc[0x11] & 0x80))
                val = (r128->atibase.svga.crtc[7] & ~0x10) | (val & 0x10);
            old                       = r128->atibase.svga.crtc[r128->atibase.svga.crtcreg];
            r128->atibase.svga.crtc[r128->atibase.svga.crtcreg] = val;
            if (old != val) {
                if (r128->atibase.svga.crtcreg < 0xe || r128->atibase.svga.crtcreg > 0x10) {
                    if ((r128->atibase.svga.crtcreg == 0xc) || (r128->atibase.svga.crtcreg == 0xd)) {
                        r128->atibase.svga.fullchange = 3;
                        r128->atibase.svga.ma_latch   = ((r128->atibase.svga.crtc[0xc] << 8) | r128->atibase.svga.crtc[0xd]) + ((r128->atibase.svga.crtc[8] & 0x60) >> 5);
                    } else {
                        r128->atibase.svga.fullchange = changeframecount;
                        svga_recalctimings(&r128->atibase.svga);
                    }
                }
            }
            break;

            if (old != val)
            {
                if (r128->atibase.svga.crtcreg < 0xE
                || r128->atibase.svga.crtcreg > 0x10)
                {
                    r128->atibase.svga.fullchange = changeframecount;
                    r128_recalc_timings(&r128->atibase.svga);
                }
            }

            break;
        default:
            svga_out(addr, val, &r128->atibase.svga);
            break;
    }
}

uint8_t
r128_svga_read(uint16_t addr, void *priv)
{
    r128_t* r128 = (r128_t*)priv;

    uint8_t ret = 0x00;

    // sanity check
    if (!r128)
        return ret; 

    // mask off b0/d0 registers 
    if ((((addr & 0xFFF0) == 0x3D0 
    || (addr & 0xFFF0) == 0x3B0) && addr < 0x3de) 
    && !(r128->atibase.svga.miscout & 1))
        addr ^= 0x60;

    switch (addr) {
        case 0x3D4:
            ret = r128->atibase.svga.crtcreg;
            break;
        case 0x3D5:
            if (r128->atibase.svga.crtcreg & 0x20)
                ret = 0xff;
            else
                ret = r128->atibase.svga.crtc[r128->atibase.svga.crtcreg];
            break;
        default:
            ret = svga_in(addr, &r128->atibase.svga);
            break;
    }
    return ret;
}

/* DFB, sets up a dumb framebuffer */
uint8_t r128_dfb_read8(uint32_t addr, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    return r128->atibase.svga.vram[addr];
}

uint16_t r128_dfb_read16(uint32_t addr, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    return (r128->atibase.svga.vram[addr + 1] << 8) | r128->atibase.svga.vram[addr];
}

uint32_t r128_dfb_read32(uint32_t addr, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    return (r128->atibase.svga.vram[addr + 3] << 24) | (r128->atibase.svga.vram[addr + 2] << 16) +
    (r128->atibase.svga.vram[addr + 1] << 8) | r128->atibase.svga.vram[addr];
}

void r128_dfb_write8(uint32_t addr, uint8_t val, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    r128->atibase.svga.vram[addr] = val;
    r128->atibase.svga.changedvram[addr >> 12] = val;
}

void r128_dfb_write16(uint32_t addr, uint16_t val, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    r128->atibase.svga.vram[addr + 1] = (val >> 8) & 0xFF;
    r128->atibase.svga.vram[addr] = (val) & 0xFF;
    r128->atibase.svga.changedvram[addr >> 12] = val;
}

void r128_dfb_write32(uint32_t addr, uint32_t val, void* priv)
{
    addr &= (r128->atibase.svga.vram_mask);
    r128->atibase.svga.vram[addr + 3] = (val >> 24) & 0xFF;
    r128->atibase.svga.vram[addr + 2] = (val >> 16) & 0xFF;
    r128->atibase.svga.vram[addr + 1] = (val >> 8) & 0xFF;
    r128->atibase.svga.vram[addr] = (val) & 0xFF;
    r128->atibase.svga.changedvram[addr >> 12] = val;
}

// Initialise the MMIO mappings
void r128_init_mappings_mmio(void)
{
    ati_log("Initialising MMIO mapping\n");

    // 0x0 - 1000000: regs
    // 0x1000000-2000000

    // initialize the mmio mapping
    mem_mapping_add(&r128->atibase.mmio_mapping, 0, 0, 
        r128_mmio_read8,
        r128_mmio_read16,
        r128_mmio_read32,
        r128_mmio_write8,
        r128_mmio_write16,
        r128_mmio_write32,
        NULL, MEM_MAPPING_EXTERNAL, r128);
}

void r128_init_mappings_svga(void)
{
    ati_log("Initialising SVGA core memory mapping\n");

    
    // setup the svga mappings
    mem_mapping_add(&r128->atibase.framebuffer_mapping, 0, 0,
        r128_dfb_read8,
        r128_dfb_read16,
        r128_dfb_read32,
        r128_dfb_write8,
        r128_dfb_write16,
        r128_dfb_write32,
        r128->atibase.svga.vram, 0, &r128->atibase.svga);

    io_sethandler(0x03c0, 0x0020, 
    r128_svga_read, NULL, NULL, 
    r128_svga_write, NULL, NULL, 
    r128);
}

void r128_init_mappings(void)
{
    r128_init_mappings_mmio();
    r128_init_mappings_svga();
}

// Updates the mappings after initialisation. 
void r128_update_mappings(void)
{
    // sanity check
    if (!r128)
        return; 

    // setting this to 0 doesn't seem to disable it, based on the datasheet

    ati_log("\nMemory Mapping Config Change:\n");

    (r128->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_IO) ? ati_log("Enable I/O\n") : ati_log("Disable I/O\n");

    io_removehandler(0x03c0, 0x0020, 
        r128_svga_read, NULL, NULL, 
        r128_svga_write, NULL, NULL, 
        r128);

    if (r128->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_IO)
        io_sethandler(0x03c0, 0x0020, 
        r128_svga_read, NULL, NULL, 
        r128_svga_write, NULL, NULL, 
        r128);   
    
    if (!(r128->pci_config.pci_regs[PCI_REG_COMMAND] & PCI_COMMAND_MEM))
    {
        ati_log("The memory was turned off, not much is going to happen.\n");
        return;
    }

    // turn off bar0 and bar1 by defualt
    mem_mapping_disable(&r128->atibase.mmio_mapping);
    mem_mapping_disable(&r128->atibase.framebuffer_mapping);
    mem_mapping_disable(&r128->atibase.framebuffer_mapping_mirror);

    // Setup BAR0 (MMIO)

    ati_log("BAR2 (MMIO Base) = 0x%08x\n", r128->atibase.bar2_mmio_base);

    
    if (r128->atibase.bar2_mmio_base)
        mem_mapping_set_addr(&r128->atibase.mmio_mapping, r128->atibase.bar2_mmio_base, R128_MMIO_SIZE);

    // if this breaks anything, remove it
    ati_log("BAR0 (Linear Framebuffer) = 0x%08x\n", r128->atibase.bar0_lfb_base);

    if (r128->atibase.bar0_lfb_base)
    {   
        mem_mapping_set_addr(&r128->atibase.framebuffer_mapping, r128->atibase.bar0_lfb_base, R128_VRAM_SIZE_64MB);
    }

    // Did we change the banked SVGA mode?
    switch (r128->atibase.svga.gdcreg[0x06] & 0x0c)
    {
        case R128_CRTC_BANKED_128K_A0000:
            ati_log("SVGA Banked Mode = 128K @ A0000h\n");
            mem_mapping_set_addr(&r128->atibase.svga.mapping, 0xA0000, 0x20000); // 128kb @ 0xA0000
            r128->atibase.svga.banked_mask = 0x1FFFF;
            break;
        case R128_CRTC_BANKED_64K_A0000:
            ati_log("SVGA Banked Mode = 64K @ A0000h\n");
            mem_mapping_set_addr(&r128->atibase.svga.mapping, 0xA0000, 0x10000); // 64kb @ 0xA0000
            r128->atibase.svga.banked_mask = 0xFFFF;
            break;
        case R128_CRTC_BANKED_32K_B0000:
            ati_log("SVGA Banked Mode = 32K @ B0000h\n");
            mem_mapping_set_addr(&r128->atibase.svga.mapping, 0xB0000, 0x8000); // 32kb @ 0xB0000
            r128->atibase.svga.banked_mask = 0x7FFF;
            break;
        case R128_CRTC_BANKED_32K_B8000:
            ati_log("SVGA Banked Mode = 32K @ B8000h\n");
            mem_mapping_set_addr(&r128->atibase.svga.mapping, 0xB8000, 0x8000); // 32kb @ 0xB8000
            r128->atibase.svga.banked_mask = 0x7FFF;
            break;
    }
}

// 
// Init code
//
void* r128_init(const device_t *info)
{
    // set the vram amount

    if (!r128->atibase.vram_amount)
        r128->atibase.vram_amount = device_get_config_int("vram_size");

#ifdef ENABLE_ATI_LOG
    // Allows ati_log to be used for multiple nvidia devices
    ati_log_set_device(r128->atibase.log); 
#endif   
    ati_log("Initialising core\n");

    // this will only be logged if ENABLE_ATI_LOG_ULTRA is defined
    ati_log_verbose_only("ULTRA LOGGING enabled");

    const char* vbios_id = device_get_config_bios("vbios");
    const char* vbios_file = "";
  
    vbios_file = device_get_bios_file(&r128_device_agp, vbios_id, 0);


    int32_t err = rom_init(&r128->atibase.vbios, vbios_file, 0xC0000, 0x10000, 0xffff, 0, MEM_MAPPING_EXTERNAL);
    
    if (err)
    {
        ati_log("ATI Rage 128 FATAL: failed to load VBIOS err=%d\n", err);
        fatal("ATI Rage 128 init failed: Somehow selected a nonexistent VBIOS? err=%d\n", err);
        return NULL;
    }
    else    
        ati_log("Successfully loaded VBIOS %s located at %s\n", vbios_id, vbios_file);

    // set up the bus and start setting up SVGA core
    if (r128->atibase.bus_generation == ati_bus_agp_4x)
    {
        ati_log("Using AGP 4X bus\n");

        pci_add_card(PCI_ADD_AGP, r128_pci_read, r128_pci_write, NULL, &r128->atibase.pci_slot);

        svga_init(&r128_device_agp, &r128->atibase.svga, r128, r128->atibase.vram_amount, 
            r128_recalc_timings, r128_svga_read, r128_svga_write, NULL, NULL);

        video_inform(VIDEO_FLAG_TYPE_SPECIAL, &timing_r128_agp);
    }

    // set vram
    ati_log("VRAM=%d bytes\n", r128->atibase.svga.vram_max);

    // init memory mappings
    r128_init_mappings();

    // make us actually exist
    r128->pci_config.int_line = 0xFF; // per datasheet
    r128->pci_config.pci_regs[PCI_REG_COMMAND] = PCI_COMMAND_IO | PCI_COMMAND_MEM;

    ati_log("Initialising I2C...");
    r128->atibase.i2c = i2c_gpio_init("r128_i2c");
    r128->atibase.ddc = ddc_init(i2c_gpio_get_bus(r128->atibase.i2c));

    return r128;
}

// Rage128 AGP initialisation function: This function simply allocates the device struct, and sets the bus to AGP before initialising.
void* r128_init_agp(const device_t* info)
{
    r128 = (r128_t*)calloc(1, sizeof(r128_t));
    if (!r128)
        return NULL;
        
    r128->atibase.bus_generation = ati_bus_agp_4x;
    return r128_init(info);
}

void r128_close(void* priv)
{
    // Shut down logging
    log_close(r128->atibase.log);
#ifdef ENABLE_ATI_LOG
    ati_log_set_device(NULL);
#endif

    // Shut down I2C and the DDC
    ddc_close(r128->atibase.ddc);
    i2c_gpio_close(r128->atibase.i2c);
    
    // Shut down SVGA
    svga_close(&r128->atibase.svga);
    free(r128);
    r128 = NULL;
}

// See if the bios rom is available.
int32_t r128_available(void)
{
    return rom_present(R128_VBIOS_RAGE128PRO_16MB)
    || rom_present(R128_VBIOS_RAGE128PRO_32MB);
}

// ATi Rage 128 Pro
// AGP
// 16MB or 32MB VRAM
const device_t r128_device_agp = 
{
    .name = "ATi Rage 128 Pro (R128) AGP",
    .internal_name = "r128_agp",
    .flags = DEVICE_AGP,
    .local = 0,
    .init = r128_init_agp,
    .close = r128_close,
    .speed_changed = r128_speed_changed,
    .force_redraw = r128_force_redraw,
    .available = r128_available,
    .config = r128_config,
};