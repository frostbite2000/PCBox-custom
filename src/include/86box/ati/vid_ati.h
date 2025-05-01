/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Shared implementation file for all ATI GPUs (hopefully to be) emulated by 86box.
 * 
 *          Credit to:
 *          - XFree86 developers                                            https://www.xfree86.org/ (esp. programs/Xserver/hw/xfree86/vga256/drivers/r128/r128_reg.h) 
 * 
 * Authors: frostbite2000
 *      
 *          Copyright 2024-2025 frostbite2000
 */
#ifdef EMU_DEVICE_H // what

//TODO: split this all into rage ii+ dvd, and r128...
#include <86box/log.h>
#include <86box/i2c.h>
#include <86box/vid_ddc.h>
#include <86box/timer.h>
#include <86box/vid_svga.h>
#include <86box/vid_svga_render.h>

void ati_log_set_device(void* device);
void ati_log(const char *fmt, ...);

// Verbose logging level.
void ati_log_verbose_only(const char *fmt, ...);

// Defines common to all ATI chip architectural generations

// PCI IDs
#define PCI_VENDOR_ATI          0x1002  // ATi PCI ID

#define ATI_PCI_NUM_CFG_REGS     256     // number of pci config registers

#define ATI_PCI_DEVICE_MACH64GTB  0x4755  // ATi 3D Rage II+ DVD (Mach 64 GTB)
#define ATI_PCI_DEVICE_R128       0x5046  // ATi Rage128 Pro/Rage Fury MAXX
#define ATI_PCI_DEVICE_R128_OTHER 0x5446  // ATi Rage128 Pro (other pci device id)

typedef enum ati_bus_generation_e
{
    // Rage II+ DVD (Mach64 GTB)
    ati_bus_pci = 1,

    // Rage 128 Pro (R128)
    ati_bus_agp_4x = 2,

} ati_bus_generation;

// ATI Base
typedef struct ati_base_s
{
    rom_t vbios;                                // ATI/OEm VBIOS
    svga_t svga;                                // SVGA core
    uint32_t vram_amount;                       // The amount of VRAM
    void* log;                                  // new logging engine
    // stuff that doesn't fit in the svga structure
    uint32_t cio_read_bank;                     // SVGA read bank
    uint32_t cio_write_bank;                    // SVGA write bank

    mem_mapping_t framebuffer_mapping;          // Linear Framebuffer
    mem_mapping_t mmio_mapping;                 // mmio mapping (32MB unified MMIO) 
    mem_mapping_t framebuffer_mapping_mirror;   // Mirror of LFB mapping
    mem_mapping_t ramin_mapping;                // RAM INput area mapping
    mem_mapping_t ramin_mapping_mirror;         // RAM INput area mapping (mirrored)
    uint8_t pci_slot;                           // pci slot number
    uint8_t pci_irq_state;                      // current PCI irq state
    uint32_t bar0_lfb_base;                     // PCI Base Address Register 0 - Linear Framebuffer
    uint32_t bar1_io_base;                      // PCI Base Address Register 1 - I/O Base
    uint32_t bar2_mmio_base;                    // PCI Base Address Register 2 - MMIO Base
    ati_bus_generation bus_generation;          // current bus
    uint32_t gpu_revision;                      // GPU Stepping
    double pixel_clock_frequency;               // Frequency used for pixel clock#
    double refresh_time;                        // Rough estimation of refresh rate, for when we can present the screen
    double refresh_clock;                       // Time since the last refresh
    bool pixel_clock_enabled;                   // Pixel Clock Enabled - stupid crap used to prevent us enabling the timer multiple times
    double memory_clock_frequency;              // Source Frequency for PTIMER
    bool memory_clock_enabled;                  // Memory Clock Enabled - stupid crap used to prevent us eanbling the timer multiple times
    void* i2c;                                  // I2C for monitor EDID
    void* ddc;                                  // Display Data Channel for EDID
    uint32_t last_buffer_address;               // Last buffer address.
} ati_base_t;

// The NV architectures are very complex.
// There are hundreds of registers at minimum, and implementing these in a standard way would lead to 
// unbelievably large switch statements and horrifically unreadable code.
// So this is used to abstract it and allow for more readable code.
// This is mostly just used for logging and stuff.
// Optionally, you can provide a function that is run when you read to and write from the register. 
// You can also implement this functionality in a traditional way such as a switch statement, for simpler registers. To do this, simply set both read and write functions to NULL.
// Typically, unless they are for a special purpose (and handled specially) e.g. vga all register reads and writes are also 32-bit aligned
typedef struct ati_register_s
{
    int32_t     address;                        // MMIO Address
    char*       friendly_name;                  // Friendly name
    // reg_ptr not needed as a parameter, because we implicitly know which register si being tiwddled
    uint32_t    (*on_read)(void);               // Optional on-read function
    void        (*on_write)(uint32_t value);    // Optional on-write fucntion
} ati_register_t; 

ati_register_t* ati_get_register(uint32_t address, ati_register_t* register_list, uint32_t num_regs);


#endif