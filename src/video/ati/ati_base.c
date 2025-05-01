/*
 * 86Box    A hypervisor and IBM PC system emulator that specializes in
 *          running old operating systems and software designed for IBM
 *          PC systems and compatibles from 1981 through fairly recent
 *          system designs based on the PCI bus.
 *
 *          This file is part of the 86Box distribution.
 *
 *          Base file for emulation of ATi video cards.
 *
 *
 *
 * Authors: frostbite2000
 *
 *          Copyright 2024 frostbite2000
 */

// Common NV1/3/4... init
#define HAVE_STDARG_H // wtf is this crap
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <86box/86box.h>
#include <86box/ati/vid_ati.h>


// Common logging
#ifdef ENABLE_ATI_LOG
int ati_do_log = ENABLE_ATI_LOG;

void ati_log(const char *fmt, ...)
{
    va_list ap;

    if (ati_do_log) {
        va_start(ap, fmt);
        pclog_ex(fmt, ap);
        va_end(ap);
    }
}
#else
void
ati_log(const char *fmt, ...)
{

}
#endif