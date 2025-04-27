/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3 3D Core Implementation
 *          Main initialization and management for the RIVA 128 / RIVA 128 ZX 3D core
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
#include <86box/nv/nv3_3d_core.h>

// Forward declarations for class initialization
extern void nv3_class_017_init(void); // D3D5 Triangle with Z-buffer
extern void nv3_class_018_init(void); // Point Z-buffer

// Initialize the 3D core
void nv3_3d_core_init(void) {
    nv_log("Initializing RIVA 128 3D Core\n");
    
    // Initialize the D3D5 triangle rendering class
    nv3_class_017_init();
    
    // Initialize the Point Z-buffer class
    nv3_class_018_init();
    
    // Other 3D core specific initialization
    // Set hardware capabilities for 3D
    nv3->pgraph.debug_2 |= (1 << NV3_PGRAPH_DEBUG_2_BILINEAR_FILTERING_3D); // Enable bilinear filtering
    nv3->pgraph.debug_2 |= (1 << NV3_PGRAPH_DEBUG_2_FOG);                  // Enable fog
    
    // The RIVA 128 doesn't have hardware T&L, so don't enable this
    // nv3->pgraph.debug_2 |= (1 << NV3_PGRAPH_DEBUG_2_LIGHTING);
    
    nv_log("RIVA 128 3D Core initialization complete\n");
}

// Reset the 3D core
void nv3_3d_core_reset(void) {
    nv_log("Resetting RIVA 128 3D Core\n");
    
    // Reset 3D core state here
    // This would be called when the GPU is reset
}

// Shutdown the 3D core
void nv3_3d_core_close(void) {
    nv_log("Shutting down RIVA 128 3D Core\n");
    
    // Clean up any allocated resources
}