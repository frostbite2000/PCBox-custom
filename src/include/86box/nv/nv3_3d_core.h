/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          NV3 3D Core Header
 *          Public interface for the RIVA 128 / RIVA 128 ZX 3D core
 *
 * Authors: Connor Hyde <mario64crashed@gmail.com>
 *
 *          Copyright 2024-2025 Connor Hyde
 */

#pragma once

// Initialize the 3D core
void nv3_3d_core_init(void);

// Reset the 3D core
void nv3_3d_core_reset(void);

// Shutdown the 3D core
void nv3_3d_core_close(void);

// Initialize 3D core integration with PGRAPH
void nv3_pgraph_3d_init(void);

// Reset the 3D core integration
void nv3_pgraph_3d_reset(void);

// Close the 3D core integration
void nv3_pgraph_3d_close(void);