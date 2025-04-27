/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3 PGRAPH 3D Core Integration
 *          Integration of 3D core functionality with the PGRAPH subsystem
 *
 * Authors: Connor Hyde <mario64crashed@gmail.com>
 *
 *          Copyright 2024-2025 Connor Hyde
 */

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <86box/86box.h>
#include <86box/device.h>
#include <86box/mem.h>
#include <86box/pci.h>
#include <86box/rom.h>
#include <86box/video.h>
#include <86box/nv/vid_nv.h>
#include <86box/nv/vid_nv3.h>
#include <86box/nv/classes/vid_nv3_classes.h>

// Forward declarations
extern void nv3_3d_core_init(void);
extern void nv3_3d_core_reset(void);
extern void nv3_3d_core_close(void);

// Initialize 3D core integration with PGRAPH
void nv3_pgraph_3d_init(void) {
    nv_log("Initializing PGRAPH 3D core integration\n");
    
    // Initialize the core 3D engine
    nv3_3d_core_init();
    
    // Configure PGRAPH for 3D operation
    
    // Enable 3D features in debug registers
    nv3->pgraph.debug_2 |= (1 << NV3_PGRAPH_DEBUG_2_BILINEAR_FILTERING_3D);   // Enable bilinear filtering
    nv3->pgraph.debug_2 |= (1 << NV3_PGRAPH_DEBUG_2_ANISOTROPIC_FILTERING_3D); // Enable anisotropic filtering
    nv3->pgraph.debug_3 |= (1 << NV3_PGRAPH_DEBUG_3_CULLING);                  // Enable culling
    
    // Enable Z-buffer related features
    nv3->pgraph.debug_3 |= (1 << NV3_PGRAPH_DEBUG_3_ZFLUSH);                   // Enable Z flushing
    nv3->pgraph.debug_3 |= (1 << NV3_PGRAPH_DEBUG_3_EARLY_ZABORT);             // Enable early Z abortion
    
    nv_log("PGRAPH 3D core integration complete\n");
}

// Reset the 3D core integration
void nv3_pgraph_3d_reset(void) {
    nv_log("Resetting PGRAPH 3D core integration\n");
    
    // Reset the core 3D engine
    nv3_3d_core_reset();
    
    // Reset PGRAPH 3D state
    nv3->pgraph.debug_3 &= ~(1 << NV3_PGRAPH_DEBUG_3_EARLY_ZABORT);
}

// Close the 3D core integration
void nv3_pgraph_3d_close(void) {
    nv_log("Closing PGRAPH 3D core integration\n");
    
    // Close the core 3D engine
    nv3_3d_core_close();
}