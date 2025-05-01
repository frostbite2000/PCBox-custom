/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 * running old operating systems and software designed for IBM
 * PC systems and compatibles from 1981 through fairly recent
 * system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          PowerVR PCX2 emulation headers.
 *
 * Authors: [Your name here],
 *
 *          Copyright 2020-2025 [Your name here].
 */

#ifndef VIDEO_POWERVR_PCX2_H
#define VIDEO_POWERVR_PCX2_H

extern int pcx2_enabled;
extern const device_t pcx2_device;

#define PVR_PCI_COMMAND_L_IO                            1
#define PVR_PCI_COMMAND_L_IO_ENABLED                    0x1
#define PVR_PCI_COMMAND_L_MEMORY                        2
#define PVR_PCI_COMMAND_L_MEMORY_ENABLED                0x1
#define PVR_PCI_COMMAND_L_MASTER					     4
#define PVR_PCI_COMMAND_L_SPECIAL					     8


#define PVR_PCI_STATUS_L_66MHZ_CAPABLE                  0x20
#define PVR_PCI_STATUS_L_FAST_BACK					     0x80
#define PVR_PCI_STATUS_L_CAP_LIST					     0x10
#define PVR_PCI_STATUS_H_DEVSEL_TIMING                  5
#define PVR_PCI_STATUS_H_FAST_DEVSEL_TIMING             0x00
#define PVR_PCI_STATUS_H_MEDIUM_DEVSEL_TIMING           0x02

#endif /* VIDEO_POWERVR_PCX2_H */