/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          PowerVR PCX2 reciprocal lookup table for fast division operations.
 *          Based on the PowerVR Series1 "Texas" rendering core.
 *
 * Authors: [Your name here],
 *
 *          Copyright 2020-2025 [Your name here].
 */

#ifndef RECTABLE_H
#define RECTABLE_H

/* 
 * Reciprocal lookup table for fast division (1/x)
 * Used by the PCX2 hardware to accelerate perspective-correct texture mapping
 * Each 16-bit entry contains a fixed-point reciprocal value
 */
extern const uint16_t pcx2_reciprocal_table[256];

#endif /* RECTABLE_H */