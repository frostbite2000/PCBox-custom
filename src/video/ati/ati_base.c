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

// Common R128... init
#define HAVE_STDARG_H // wtf is this crap
#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <86box/86box.h>
#include <86box/ati/vid_ati.h>


// Common logging
#ifdef ENABLE_ATI_LOG
int ati_do_log = ENABLE_ATI_LOG;

void* ati_log_device;
bool ati_log_full = false;

void ati_log_set_device(void* device)
{
    #ifndef RELEASE_BUILD
    if (device 
    && device_get_config_int("ati_debug_fulllog"))
    {
        ati_log_full = true; 
    }
    #endif

    ati_log_device = device;
}

void ati_log_internal(const char* fmt, va_list arg)
{
    if (!ati_log_device)
        return;

    // If our debug config option is configured, full log. Otherwise log with cyclical detection.
    if (ati_log_full)   
        log_out(ati_log_device, fmt, arg);
    else
        log_out_cyclic(ati_log_device, fmt, arg);
    
}

void ati_log(const char *fmt, ...)
{
    va_list arg; 

    if (!ati_do_log)
        return; 

    va_start(arg, fmt);
    ati_log_internal(fmt, arg);
    va_end(arg);
}

void ati_log_verbose_only(const char *fmt, ...)
{
    #ifdef ENABLE_ati_LOG_ULTRA
    va_list arg; 

    if (!ati_do_log)
        return; 

    va_start(arg, fmt);
    ati_log_internal(fmt, arg);
    va_end(arg);
    #endif
}

#else
void ati_log(const char *fmt, ...)
{
    
}

void ati_log_verbose_only(const char *fmt, ...)
{
    
}

void ati_log_set_device(void* device)
{

}
#endif