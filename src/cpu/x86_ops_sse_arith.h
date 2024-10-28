/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdlib.h>
#include <math.h>

static int
opSQRTPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSQRTPD_xmm_xmm_a16(fetchdat);
    
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = sqrt(cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = sqrt(cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = sqrt(cpu_state_high.XMM[cpu_rm].f[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = sqrt(src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = sqrt(src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = sqrt(src_real[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opSQRTPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSQRTPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = sqrt(cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = sqrt(cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = sqrt(cpu_state_high.XMM[cpu_rm].f[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = sqrt(src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = sqrt(src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = sqrt(src_real[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opSQRTSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(src_real);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opSQRTSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = sqrt(src_real);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRSQRTPS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / sqrt(src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / sqrt(src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / sqrt(src_real[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opRSQRTPS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / sqrt(src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / sqrt(src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / sqrt(src_real[3]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRSQRTSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(src_real);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRSQRTSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(cpu_state_high.XMM[cpu_rm].f[0]);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / sqrt(src_real);
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRCPSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRCPSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opRCPPS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / cpu_state_high.XMM[cpu_rm].f[3];
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0]       = *(float *) &src[0];
        src_real[1]       = *(float *) &src[1];
        src_real[2]       = *(float *) &src[2];
        src_real[3]       = *(float *) &src[3];
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / src_real[3];
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opRCPPS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] = 1.0 / src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] = 1.0 / src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] = 1.0 / src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] = 1.0 / src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opADDPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opADDPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] += cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] += cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] += cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] += src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] += src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] += src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opADDPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opADDPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] += cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] += cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] += cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] += src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] += src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] += src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opADDSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opADDSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] += src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMULPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMULPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] *= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] *= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] *= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] *= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] *= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] *= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMULPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMULPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] *= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] *= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] *= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] *= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] *= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] *= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMULSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMULSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] *= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opSUBPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSUBPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] -= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] -= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] -= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] -= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] -= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] -= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opSUBPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSUBPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] -= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] -= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] -= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] -= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] -= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] -= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opSUBSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opSUBSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] -= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMINPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMINPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmin(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmin(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmin(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0]       = *(float *) &src[0];
        src_real[1]       = *(float *) &src[1];
        src_real[2]       = *(float *) &src[2];
        src_real[3]       = *(float *) &src[3];
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmin(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmin(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmin(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMINPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMINPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmin(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmin(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmin(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0]       = *(float *) &src[0];
        src_real[1]       = *(float *) &src[1];
        src_real[2]       = *(float *) &src[2];
        src_real[3]       = *(float *) &src[3];
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmin(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmin(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmin(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMINSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real          = *(float *) &src;
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], src_real);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMINSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real          = *(float *) &src;
        cpu_state_high.XMM[cpu_reg].f[0] = fmin(cpu_state_high.XMM[cpu_reg].f[0], src_real);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opDIVPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opDIVPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] /= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] /= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] /= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] /= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] /= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] /= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opDIVPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opDIVPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= cpu_state_high.XMM[cpu_rm].f[0];
        cpu_state_high.XMM[cpu_reg].f[1] /= cpu_state_high.XMM[cpu_rm].f[1];
        cpu_state_high.XMM[cpu_reg].f[2] /= cpu_state_high.XMM[cpu_rm].f[2];
        cpu_state_high.XMM[cpu_reg].f[3] /= cpu_state_high.XMM[cpu_rm].f[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= src_real[0];
        cpu_state_high.XMM[cpu_reg].f[1] /= src_real[1];
        cpu_state_high.XMM[cpu_reg].f[2] /= src_real[2];
        cpu_state_high.XMM[cpu_reg].f[3] /= src_real[3];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opDIVSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opDIVSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= cpu_state_high.XMM[cpu_rm].f[0];
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f[0] /= src_real;
        fesetround(FE_TONEAREST);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMAXPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMAXPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmax(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmax(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmax(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0]       = *(float *) &src[0];
        src_real[1]       = *(float *) &src[1];
        src_real[2]       = *(float *) &src[2];
        src_real[3]       = *(float *) &src[3];
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmax(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmax(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmax(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMAXPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMAXPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmax(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmax(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmax(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0]       = *(float *) &src[0];
        src_real[1]       = *(float *) &src[1];
        src_real[2]       = *(float *) &src[2];
        src_real[3]       = *(float *) &src[3];
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]);
        cpu_state_high.XMM[cpu_reg].f[1] = fmax(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]);
        cpu_state_high.XMM[cpu_reg].f[2] = fmax(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]);
        cpu_state_high.XMM[cpu_reg].f[3] = fmax(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[1]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[2]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[3]);
    }

    return 0;
}

static int
opMAXSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real          = *(float *) &src;
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], src_real);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opMAXSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real          = *(float *) &src;
        cpu_state_high.XMM[cpu_reg].f[0] = fmax(cpu_state_high.XMM[cpu_reg].f[0], src_real);
        check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f[0]);
    }

    return 0;
}

static int
opCMPPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opCMPPD_xmm_xmm_a16(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    uint8_t  imm = getbyte();
    uint32_t cmp[4] = { 0, 0, 0, 0 };
    if (cpu_mod == 3) {
        switch (imm & 7) {
            case 0:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] == cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] == cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] == cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] == cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] < cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] < cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] < cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] <= cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] <= cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] <= cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] != cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] != cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] != cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] != cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] < cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] < cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] < cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] <= cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] <= cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] <= cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? 0 : ~0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]) ? 0 : ~0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]) ? 0 : ~0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp[0];
        cpu_state_high.XMM[cpu_reg].l[1] = cmp[1];
        cpu_state_high.XMM[cpu_reg].l[2] = cmp[2];
        cpu_state_high.XMM[cpu_reg].l[3] = cmp[3];
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        switch (imm & 7) {
            case 0:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] == src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] == src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] == src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] == src_real[3] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] < src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] < src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] < src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] < src_real[3] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] <= src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] <= src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] <= src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] <= src_real[3] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]) ? ~0 : 0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]) ? ~0 : 0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]) ? ~0 : 0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] != src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] != src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] != src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] != src_real[3] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] < src_real[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] < src_real[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] < src_real[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] < src_real[3]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] <= src_real[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] <= src_real[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] <= src_real[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] <= src_real[3]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]) ? 0 : ~0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]) ? 0 : ~0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]) ? 0 : ~0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp[0];
        cpu_state_high.XMM[cpu_reg].l[1] = cmp[1];
        cpu_state_high.XMM[cpu_reg].l[2] = cmp[2];
        cpu_state_high.XMM[cpu_reg].l[3] = cmp[3];
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opCMPPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opCMPPD_xmm_xmm_a32(fetchdat);

    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    uint8_t  imm = getbyte();
    uint32_t cmp[4] = { 0, 0, 0, 0 };
    if (cpu_mod == 3) {
        switch (imm & 7) {
            case 0:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] == cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] == cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] == cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] == cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] < cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] < cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] < cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] <= cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] <= cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] <= cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] != cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] != cpu_state_high.XMM[cpu_rm].f[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] != cpu_state_high.XMM[cpu_rm].f[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] != cpu_state_high.XMM[cpu_rm].f[3] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] < cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] < cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] < cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] <= cpu_state_high.XMM[cpu_rm].f[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] <= cpu_state_high.XMM[cpu_rm].f[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] <= cpu_state_high.XMM[cpu_rm].f[3]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? 0 : ~0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], cpu_state_high.XMM[cpu_rm].f[1]) ? 0 : ~0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], cpu_state_high.XMM[cpu_rm].f[2]) ? 0 : ~0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], cpu_state_high.XMM[cpu_rm].f[3]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp[0];
        cpu_state_high.XMM[cpu_reg].l[1] = cmp[1];
        cpu_state_high.XMM[cpu_reg].l[2] = cmp[2];
        cpu_state_high.XMM[cpu_reg].l[3] = cmp[3];
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float src_real[4];
        src_real[0] = *(float *) &src[0];
        src_real[1] = *(float *) &src[1];
        src_real[2] = *(float *) &src[2];
        src_real[3] = *(float *) &src[3];
        switch (imm & 7) {
            case 0:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] == src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] == src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] == src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] == src_real[3] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] < src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] < src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] < src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] < src_real[3] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] <= src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] <= src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] <= src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] <= src_real[3] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]) ? ~0 : 0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]) ? ~0 : 0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]) ? ~0 : 0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp[0] = cpu_state_high.XMM[cpu_reg].f[0] != src_real[0] ? ~0 : 0;
                    cmp[1] = cpu_state_high.XMM[cpu_reg].f[1] != src_real[1] ? ~0 : 0;
                    cmp[2] = cpu_state_high.XMM[cpu_reg].f[2] != src_real[2] ? ~0 : 0;
                    cmp[3] = cpu_state_high.XMM[cpu_reg].f[3] != src_real[3] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] < src_real[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] < src_real[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] < src_real[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] < src_real[3]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp[0] = !(cpu_state_high.XMM[cpu_reg].f[0] <= src_real[0]) ? ~0 : 0;
                    cmp[1] = !(cpu_state_high.XMM[cpu_reg].f[1] <= src_real[1]) ? ~0 : 0;
                    cmp[2] = !(cpu_state_high.XMM[cpu_reg].f[2] <= src_real[2]) ? ~0 : 0;
                    cmp[3] = !(cpu_state_high.XMM[cpu_reg].f[3] <= src_real[3]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp[0] = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real[0]) ? 0 : ~0;
                    cmp[1] = isunordered(cpu_state_high.XMM[cpu_reg].f[1], src_real[1]) ? 0 : ~0;
                    cmp[2] = isunordered(cpu_state_high.XMM[cpu_reg].f[2], src_real[2]) ? 0 : ~0;
                    cmp[3] = isunordered(cpu_state_high.XMM[cpu_reg].f[3], src_real[3]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp[0];
        cpu_state_high.XMM[cpu_reg].l[1] = cmp[1];
        cpu_state_high.XMM[cpu_reg].l[2] = cmp[2];
        cpu_state_high.XMM[cpu_reg].l[3] = cmp[3];
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opCMPSS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    uint8_t  imm = getbyte();
    uint32_t cmp = 0;
    if (cpu_mod == 3) {
        switch (imm & 7) {
            case 0:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] == cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] != cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp;
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        switch (imm & 7) {
            case 0:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] == src_real ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] < src_real ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] <= src_real ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] != src_real ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] < src_real) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] <= src_real) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp;
        CLOCK_CYCLES(2);
    }

    return 0;
}

static int
opCMPSS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    uint8_t  imm = getbyte();
    uint32_t cmp = 0;
    if (cpu_mod == 3) {
        switch (imm & 7) {
            case 0:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] == cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] != cpu_state_high.XMM[cpu_rm].f[0] ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] < cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] <= cpu_state_high.XMM[cpu_rm].f[0]) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], cpu_state_high.XMM[cpu_rm].f[0]) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp;
        CLOCK_CYCLES(1);
    } else {
        uint32_t src;

        SEG_CHECK_READ(cpu_state.ea_seg);
        src = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float src_real;
        src_real = *(float *) &src;
        switch (imm & 7) {
            case 0:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] == src_real ? ~0 : 0;
                    break;
                }
            case 1:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] < src_real ? ~0 : 0;
                    break;
                }
            case 2:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] <= src_real ? ~0 : 0;
                    break;
                }
            case 3:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real) ? ~0 : 0;
                    break;
                }
            case 4:
                {
                    cmp = cpu_state_high.XMM[cpu_reg].f[0] != src_real ? ~0 : 0;
                    break;
                }
            case 5:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] < src_real) ? ~0 : 0;
                    break;
                }
            case 6:
                {
                    cmp = !(cpu_state_high.XMM[cpu_reg].f[0] <= src_real) ? ~0 : 0;
                    break;
                }
            case 7:
                {
                    cmp = isunordered(cpu_state_high.XMM[cpu_reg].f[0], src_real) ? 0 : ~0;
                    break;
                }

            default:
                break;
        }
        cpu_state_high.XMM[cpu_reg].l[0] = cmp;
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opPMINUB_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->b[0] = fmin(dst->b[0], src.b[0]);
    dst->b[1] = fmin(dst->b[1], src.b[1]);
    dst->b[2] = fmin(dst->b[2], src.b[2]);
    dst->b[3] = fmin(dst->b[3], src.b[3]);
    dst->b[4] = fmin(dst->b[4], src.b[4]);
    dst->b[5] = fmin(dst->b[5], src.b[5]);
    dst->b[6] = fmin(dst->b[6], src.b[6]);
    dst->b[7] = fmin(dst->b[7], src.b[7]);

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMINUB_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);
    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->b[0] = fmin(dst->b[0], src.b[0]);
    dst->b[1] = fmin(dst->b[1], src.b[1]);
    dst->b[2] = fmin(dst->b[2], src.b[2]);
    dst->b[3] = fmin(dst->b[3], src.b[3]);
    dst->b[4] = fmin(dst->b[4], src.b[4]);
    dst->b[5] = fmin(dst->b[5], src.b[5]);
    dst->b[6] = fmin(dst->b[6], src.b[6]);
    dst->b[7] = fmin(dst->b[7], src.b[7]);

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMINUB_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].b[0]  = fmin(cpu_state_high.XMM[cpu_reg].b[0], cpu_state_high.XMM[cpu_rm].b[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmin(cpu_state_high.XMM[cpu_reg].b[1], cpu_state_high.XMM[cpu_rm].b[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmin(cpu_state_high.XMM[cpu_reg].b[2], cpu_state_high.XMM[cpu_rm].b[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmin(cpu_state_high.XMM[cpu_reg].b[3], cpu_state_high.XMM[cpu_rm].b[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmin(cpu_state_high.XMM[cpu_reg].b[4], cpu_state_high.XMM[cpu_rm].b[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmin(cpu_state_high.XMM[cpu_reg].b[5], cpu_state_high.XMM[cpu_rm].b[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmin(cpu_state_high.XMM[cpu_reg].b[6], cpu_state_high.XMM[cpu_rm].b[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmin(cpu_state_high.XMM[cpu_reg].b[7], cpu_state_high.XMM[cpu_rm].b[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmin(cpu_state_high.XMM[cpu_reg].b[8], cpu_state_high.XMM[cpu_rm].b[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmin(cpu_state_high.XMM[cpu_reg].b[9], cpu_state_high.XMM[cpu_rm].b[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmin(cpu_state_high.XMM[cpu_reg].b[10], cpu_state_high.XMM[cpu_rm].b[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmin(cpu_state_high.XMM[cpu_reg].b[11], cpu_state_high.XMM[cpu_rm].b[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmin(cpu_state_high.XMM[cpu_reg].b[12], cpu_state_high.XMM[cpu_rm].b[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmin(cpu_state_high.XMM[cpu_reg].b[13], cpu_state_high.XMM[cpu_rm].b[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmin(cpu_state_high.XMM[cpu_reg].b[14], cpu_state_high.XMM[cpu_rm].b[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmin(cpu_state_high.XMM[cpu_reg].b[15], cpu_state_high.XMM[cpu_rm].b[15]);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].b[0]  = fmin(cpu_state_high.XMM[cpu_reg].b[0], src[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmin(cpu_state_high.XMM[cpu_reg].b[1], src[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmin(cpu_state_high.XMM[cpu_reg].b[2], src[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmin(cpu_state_high.XMM[cpu_reg].b[3], src[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmin(cpu_state_high.XMM[cpu_reg].b[4], src[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmin(cpu_state_high.XMM[cpu_reg].b[5], src[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmin(cpu_state_high.XMM[cpu_reg].b[6], src[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmin(cpu_state_high.XMM[cpu_reg].b[7], src[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmin(cpu_state_high.XMM[cpu_reg].b[8], src[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmin(cpu_state_high.XMM[cpu_reg].b[9], src[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmin(cpu_state_high.XMM[cpu_reg].b[10], src[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmin(cpu_state_high.XMM[cpu_reg].b[11], src[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmin(cpu_state_high.XMM[cpu_reg].b[12], src[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmin(cpu_state_high.XMM[cpu_reg].b[13], src[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmin(cpu_state_high.XMM[cpu_reg].b[14], src[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmin(cpu_state_high.XMM[cpu_reg].b[15], src[15]);
    }

    return 0;
}

static int
opPMINUB_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].b[0]  = fmin(cpu_state_high.XMM[cpu_reg].b[0], cpu_state_high.XMM[cpu_rm].b[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmin(cpu_state_high.XMM[cpu_reg].b[1], cpu_state_high.XMM[cpu_rm].b[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmin(cpu_state_high.XMM[cpu_reg].b[2], cpu_state_high.XMM[cpu_rm].b[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmin(cpu_state_high.XMM[cpu_reg].b[3], cpu_state_high.XMM[cpu_rm].b[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmin(cpu_state_high.XMM[cpu_reg].b[4], cpu_state_high.XMM[cpu_rm].b[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmin(cpu_state_high.XMM[cpu_reg].b[5], cpu_state_high.XMM[cpu_rm].b[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmin(cpu_state_high.XMM[cpu_reg].b[6], cpu_state_high.XMM[cpu_rm].b[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmin(cpu_state_high.XMM[cpu_reg].b[7], cpu_state_high.XMM[cpu_rm].b[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmin(cpu_state_high.XMM[cpu_reg].b[8], cpu_state_high.XMM[cpu_rm].b[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmin(cpu_state_high.XMM[cpu_reg].b[9], cpu_state_high.XMM[cpu_rm].b[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmin(cpu_state_high.XMM[cpu_reg].b[10], cpu_state_high.XMM[cpu_rm].b[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmin(cpu_state_high.XMM[cpu_reg].b[11], cpu_state_high.XMM[cpu_rm].b[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmin(cpu_state_high.XMM[cpu_reg].b[12], cpu_state_high.XMM[cpu_rm].b[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmin(cpu_state_high.XMM[cpu_reg].b[13], cpu_state_high.XMM[cpu_rm].b[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmin(cpu_state_high.XMM[cpu_reg].b[14], cpu_state_high.XMM[cpu_rm].b[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmin(cpu_state_high.XMM[cpu_reg].b[15], cpu_state_high.XMM[cpu_rm].b[15]);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].b[0]  = fmin(cpu_state_high.XMM[cpu_reg].b[0], src[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmin(cpu_state_high.XMM[cpu_reg].b[1], src[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmin(cpu_state_high.XMM[cpu_reg].b[2], src[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmin(cpu_state_high.XMM[cpu_reg].b[3], src[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmin(cpu_state_high.XMM[cpu_reg].b[4], src[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmin(cpu_state_high.XMM[cpu_reg].b[5], src[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmin(cpu_state_high.XMM[cpu_reg].b[6], src[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmin(cpu_state_high.XMM[cpu_reg].b[7], src[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmin(cpu_state_high.XMM[cpu_reg].b[8], src[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmin(cpu_state_high.XMM[cpu_reg].b[9], src[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmin(cpu_state_high.XMM[cpu_reg].b[10], src[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmin(cpu_state_high.XMM[cpu_reg].b[11], src[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmin(cpu_state_high.XMM[cpu_reg].b[12], src[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmin(cpu_state_high.XMM[cpu_reg].b[13], src[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmin(cpu_state_high.XMM[cpu_reg].b[14], src[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmin(cpu_state_high.XMM[cpu_reg].b[15], src[15]);
    }

    return 0;
}

static int
opPMINUB_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMINUB_xmm_xmm_a16(fetchdat);
    else
        return opPMINUB_mm_mm_a16(fetchdat);
}

static int
opPMINUB_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMINUB_xmm_xmm_a32(fetchdat);
    else
        return opPMINUB_mm_mm_a32(fetchdat);
}

static int
opPMAXUB_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->b[0] = fmax(dst->b[0], src.b[0]);
    dst->b[1] = fmax(dst->b[1], src.b[1]);
    dst->b[2] = fmax(dst->b[2], src.b[2]);
    dst->b[3] = fmax(dst->b[3], src.b[3]);
    dst->b[4] = fmax(dst->b[4], src.b[4]);
    dst->b[5] = fmax(dst->b[5], src.b[5]);
    dst->b[6] = fmax(dst->b[6], src.b[6]);
    dst->b[7] = fmax(dst->b[7], src.b[7]);

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMAXUB_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->b[0] = fmax(dst->b[0], src.b[0]);
    dst->b[1] = fmax(dst->b[1], src.b[1]);
    dst->b[2] = fmax(dst->b[2], src.b[2]);
    dst->b[3] = fmax(dst->b[3], src.b[3]);
    dst->b[4] = fmax(dst->b[4], src.b[4]);
    dst->b[5] = fmax(dst->b[5], src.b[5]);
    dst->b[6] = fmax(dst->b[6], src.b[6]);
    dst->b[7] = fmax(dst->b[7], src.b[7]);

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMAXUB_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].b[0]  = fmax(cpu_state_high.XMM[cpu_reg].b[0], cpu_state_high.XMM[cpu_rm].b[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmax(cpu_state_high.XMM[cpu_reg].b[1], cpu_state_high.XMM[cpu_rm].b[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmax(cpu_state_high.XMM[cpu_reg].b[2], cpu_state_high.XMM[cpu_rm].b[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmax(cpu_state_high.XMM[cpu_reg].b[3], cpu_state_high.XMM[cpu_rm].b[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmax(cpu_state_high.XMM[cpu_reg].b[4], cpu_state_high.XMM[cpu_rm].b[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmax(cpu_state_high.XMM[cpu_reg].b[5], cpu_state_high.XMM[cpu_rm].b[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmax(cpu_state_high.XMM[cpu_reg].b[6], cpu_state_high.XMM[cpu_rm].b[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmax(cpu_state_high.XMM[cpu_reg].b[7], cpu_state_high.XMM[cpu_rm].b[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmax(cpu_state_high.XMM[cpu_reg].b[8], cpu_state_high.XMM[cpu_rm].b[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmax(cpu_state_high.XMM[cpu_reg].b[9], cpu_state_high.XMM[cpu_rm].b[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmax(cpu_state_high.XMM[cpu_reg].b[10], cpu_state_high.XMM[cpu_rm].b[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmax(cpu_state_high.XMM[cpu_reg].b[11], cpu_state_high.XMM[cpu_rm].b[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmax(cpu_state_high.XMM[cpu_reg].b[12], cpu_state_high.XMM[cpu_rm].b[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmax(cpu_state_high.XMM[cpu_reg].b[13], cpu_state_high.XMM[cpu_rm].b[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmax(cpu_state_high.XMM[cpu_reg].b[14], cpu_state_high.XMM[cpu_rm].b[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmax(cpu_state_high.XMM[cpu_reg].b[15], cpu_state_high.XMM[cpu_rm].b[15]);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].b[0]  = fmax(cpu_state_high.XMM[cpu_reg].b[0], src[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmax(cpu_state_high.XMM[cpu_reg].b[1], src[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmax(cpu_state_high.XMM[cpu_reg].b[2], src[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmax(cpu_state_high.XMM[cpu_reg].b[3], src[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmax(cpu_state_high.XMM[cpu_reg].b[4], src[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmax(cpu_state_high.XMM[cpu_reg].b[5], src[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmax(cpu_state_high.XMM[cpu_reg].b[6], src[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmax(cpu_state_high.XMM[cpu_reg].b[7], src[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmax(cpu_state_high.XMM[cpu_reg].b[8], src[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmax(cpu_state_high.XMM[cpu_reg].b[9], src[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmax(cpu_state_high.XMM[cpu_reg].b[10], src[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmax(cpu_state_high.XMM[cpu_reg].b[11], src[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmax(cpu_state_high.XMM[cpu_reg].b[12], src[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmax(cpu_state_high.XMM[cpu_reg].b[13], src[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmax(cpu_state_high.XMM[cpu_reg].b[14], src[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmax(cpu_state_high.XMM[cpu_reg].b[15], src[15]);
    }

    return 0;
}

static int
opPMAXUB_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].b[0]  = fmax(cpu_state_high.XMM[cpu_reg].b[0], cpu_state_high.XMM[cpu_rm].b[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmax(cpu_state_high.XMM[cpu_reg].b[1], cpu_state_high.XMM[cpu_rm].b[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmax(cpu_state_high.XMM[cpu_reg].b[2], cpu_state_high.XMM[cpu_rm].b[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmax(cpu_state_high.XMM[cpu_reg].b[3], cpu_state_high.XMM[cpu_rm].b[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmax(cpu_state_high.XMM[cpu_reg].b[4], cpu_state_high.XMM[cpu_rm].b[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmax(cpu_state_high.XMM[cpu_reg].b[5], cpu_state_high.XMM[cpu_rm].b[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmax(cpu_state_high.XMM[cpu_reg].b[6], cpu_state_high.XMM[cpu_rm].b[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmax(cpu_state_high.XMM[cpu_reg].b[7], cpu_state_high.XMM[cpu_rm].b[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmax(cpu_state_high.XMM[cpu_reg].b[8], cpu_state_high.XMM[cpu_rm].b[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmax(cpu_state_high.XMM[cpu_reg].b[9], cpu_state_high.XMM[cpu_rm].b[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmax(cpu_state_high.XMM[cpu_reg].b[10], cpu_state_high.XMM[cpu_rm].b[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmax(cpu_state_high.XMM[cpu_reg].b[11], cpu_state_high.XMM[cpu_rm].b[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmax(cpu_state_high.XMM[cpu_reg].b[12], cpu_state_high.XMM[cpu_rm].b[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmax(cpu_state_high.XMM[cpu_reg].b[13], cpu_state_high.XMM[cpu_rm].b[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmax(cpu_state_high.XMM[cpu_reg].b[14], cpu_state_high.XMM[cpu_rm].b[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmax(cpu_state_high.XMM[cpu_reg].b[15], cpu_state_high.XMM[cpu_rm].b[15]);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].b[0]  = fmax(cpu_state_high.XMM[cpu_reg].b[0], src[0]);
        cpu_state_high.XMM[cpu_reg].b[1]  = fmax(cpu_state_high.XMM[cpu_reg].b[1], src[1]);
        cpu_state_high.XMM[cpu_reg].b[2]  = fmax(cpu_state_high.XMM[cpu_reg].b[2], src[2]);
        cpu_state_high.XMM[cpu_reg].b[3]  = fmax(cpu_state_high.XMM[cpu_reg].b[3], src[3]);
        cpu_state_high.XMM[cpu_reg].b[4]  = fmax(cpu_state_high.XMM[cpu_reg].b[4], src[4]);
        cpu_state_high.XMM[cpu_reg].b[5]  = fmax(cpu_state_high.XMM[cpu_reg].b[5], src[5]);
        cpu_state_high.XMM[cpu_reg].b[6]  = fmax(cpu_state_high.XMM[cpu_reg].b[6], src[6]);
        cpu_state_high.XMM[cpu_reg].b[7]  = fmax(cpu_state_high.XMM[cpu_reg].b[7], src[7]);
        cpu_state_high.XMM[cpu_reg].b[8]  = fmax(cpu_state_high.XMM[cpu_reg].b[8], src[8]);
        cpu_state_high.XMM[cpu_reg].b[9]  = fmax(cpu_state_high.XMM[cpu_reg].b[9], src[9]);
        cpu_state_high.XMM[cpu_reg].b[10] = fmax(cpu_state_high.XMM[cpu_reg].b[10], src[10]);
        cpu_state_high.XMM[cpu_reg].b[11] = fmax(cpu_state_high.XMM[cpu_reg].b[11], src[11]);
        cpu_state_high.XMM[cpu_reg].b[12] = fmax(cpu_state_high.XMM[cpu_reg].b[12], src[12]);
        cpu_state_high.XMM[cpu_reg].b[13] = fmax(cpu_state_high.XMM[cpu_reg].b[13], src[13]);
        cpu_state_high.XMM[cpu_reg].b[14] = fmax(cpu_state_high.XMM[cpu_reg].b[14], src[14]);
        cpu_state_high.XMM[cpu_reg].b[15] = fmax(cpu_state_high.XMM[cpu_reg].b[15], src[15]);
    }

    return 0;
}

static int
opPMAXUB_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMAXUB_xmm_xmm_a16(fetchdat);
    else
        return opPMAXUB_mm_mm_a16(fetchdat);
}

static int
opPMAXUB_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMAXUB_xmm_xmm_a32(fetchdat);
    else
        return opPMAXUB_mm_mm_a32(fetchdat);
}

static int
opPAVGB_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    uint16_t tmp[8];

    tmp[0] = dst->b[0] + src.b[0] + 1;
    tmp[1] = dst->b[1] + src.b[1] + 1;
    tmp[2] = dst->b[2] + src.b[2] + 1;
    tmp[3] = dst->b[3] + src.b[3] + 1;
    tmp[4] = dst->b[4] + src.b[4] + 1;
    tmp[5] = dst->b[5] + src.b[5] + 1;
    tmp[6] = dst->b[6] + src.b[6] + 1;
    tmp[7] = dst->b[7] + src.b[7] + 1;

    dst->b[0] = (uint8_t) (tmp[0] >> 1);
    dst->b[1] = (uint8_t) (tmp[1] >> 1);
    dst->b[2] = (uint8_t) (tmp[2] >> 1);
    dst->b[3] = (uint8_t) (tmp[3] >> 1);
    dst->b[4] = (uint8_t) (tmp[4] >> 1);
    dst->b[5] = (uint8_t) (tmp[5] >> 1);
    dst->b[6] = (uint8_t) (tmp[6] >> 1);
    dst->b[7] = (uint8_t) (tmp[7] >> 1);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPAVGB_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    uint16_t tmp[8];

    tmp[0] = dst->b[0] + src.b[0] + 1;
    tmp[1] = dst->b[1] + src.b[1] + 1;
    tmp[2] = dst->b[2] + src.b[2] + 1;
    tmp[3] = dst->b[3] + src.b[3] + 1;
    tmp[4] = dst->b[4] + src.b[4] + 1;
    tmp[5] = dst->b[5] + src.b[5] + 1;
    tmp[6] = dst->b[6] + src.b[6] + 1;
    tmp[7] = dst->b[7] + src.b[7] + 1;

    dst->b[0] = (uint8_t) (tmp[0] >> 1);
    dst->b[1] = (uint8_t) (tmp[1] >> 1);
    dst->b[2] = (uint8_t) (tmp[2] >> 1);
    dst->b[3] = (uint8_t) (tmp[3] >> 1);
    dst->b[4] = (uint8_t) (tmp[4] >> 1);
    dst->b[5] = (uint8_t) (tmp[5] >> 1);
    dst->b[6] = (uint8_t) (tmp[6] >> 1);
    dst->b[7] = (uint8_t) (tmp[7] >> 1);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPAVGB_xmm_xmm_a16(uint32_t fetchdat)
{
    uint16_t tmp[16];
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        tmp[0]  = cpu_state_high.XMM[cpu_reg].b[0] + cpu_state_high.XMM[cpu_rm].b[0] + 1;
        tmp[1]  = cpu_state_high.XMM[cpu_reg].b[1] + cpu_state_high.XMM[cpu_rm].b[1] + 1;
        tmp[2]  = cpu_state_high.XMM[cpu_reg].b[2] + cpu_state_high.XMM[cpu_rm].b[2] + 1;
        tmp[3]  = cpu_state_high.XMM[cpu_reg].b[3] + cpu_state_high.XMM[cpu_rm].b[3] + 1;
        tmp[4]  = cpu_state_high.XMM[cpu_reg].b[4] + cpu_state_high.XMM[cpu_rm].b[4] + 1;
        tmp[5]  = cpu_state_high.XMM[cpu_reg].b[5] + cpu_state_high.XMM[cpu_rm].b[5] + 1;
        tmp[6]  = cpu_state_high.XMM[cpu_reg].b[6] + cpu_state_high.XMM[cpu_rm].b[6] + 1;
        tmp[7]  = cpu_state_high.XMM[cpu_reg].b[7] + cpu_state_high.XMM[cpu_rm].b[7] + 1;
        tmp[8]  = cpu_state_high.XMM[cpu_reg].b[8] + cpu_state_high.XMM[cpu_rm].b[8] + 1;
        tmp[9]  = cpu_state_high.XMM[cpu_reg].b[9] + cpu_state_high.XMM[cpu_rm].b[9] + 1;
        tmp[10] = cpu_state_high.XMM[cpu_reg].b[10] + cpu_state_high.XMM[cpu_rm].b[10] + 1;
        tmp[11] = cpu_state_high.XMM[cpu_reg].b[11] + cpu_state_high.XMM[cpu_rm].b[11] + 1;
        tmp[12] = cpu_state_high.XMM[cpu_reg].b[12] + cpu_state_high.XMM[cpu_rm].b[12] + 1;
        tmp[13] = cpu_state_high.XMM[cpu_reg].b[13] + cpu_state_high.XMM[cpu_rm].b[13] + 1;
        tmp[14] = cpu_state_high.XMM[cpu_reg].b[14] + cpu_state_high.XMM[cpu_rm].b[14] + 1;
        tmp[15] = cpu_state_high.XMM[cpu_reg].b[15] + cpu_state_high.XMM[cpu_rm].b[15] + 1;

        cpu_state_high.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state_high.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state_high.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state_high.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state_high.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state_high.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state_high.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state_high.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state_high.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;

        tmp[0]  = cpu_state_high.XMM[cpu_reg].b[0] + src[0] + 1;
        tmp[1]  = cpu_state_high.XMM[cpu_reg].b[1] + src[1] + 1;
        tmp[2]  = cpu_state_high.XMM[cpu_reg].b[2] + src[2] + 1;
        tmp[3]  = cpu_state_high.XMM[cpu_reg].b[3] + src[3] + 1;
        tmp[4]  = cpu_state_high.XMM[cpu_reg].b[4] + src[4] + 1;
        tmp[5]  = cpu_state_high.XMM[cpu_reg].b[5] + src[5] + 1;
        tmp[6]  = cpu_state_high.XMM[cpu_reg].b[6] + src[6] + 1;
        tmp[7]  = cpu_state_high.XMM[cpu_reg].b[7] + src[7] + 1;
        tmp[8]  = cpu_state_high.XMM[cpu_reg].b[8] + src[8] + 1;
        tmp[9]  = cpu_state_high.XMM[cpu_reg].b[9] + src[9] + 1;
        tmp[10] = cpu_state_high.XMM[cpu_reg].b[10] + src[10] + 1;
        tmp[11] = cpu_state_high.XMM[cpu_reg].b[11] + src[11] + 1;
        tmp[12] = cpu_state_high.XMM[cpu_reg].b[12] + src[12] + 1;
        tmp[13] = cpu_state_high.XMM[cpu_reg].b[13] + src[13] + 1;
        tmp[14] = cpu_state_high.XMM[cpu_reg].b[14] + src[14] + 1;
        tmp[15] = cpu_state_high.XMM[cpu_reg].b[15] + src[15] + 1;

        cpu_state_high.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state_high.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state_high.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state_high.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state_high.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state_high.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state_high.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state_high.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state_high.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
    }

    return 0;
}

static int
opPAVGB_xmm_xmm_a32(uint32_t fetchdat)
{
    uint16_t tmp[16];
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        tmp[0]  = cpu_state_high.XMM[cpu_reg].b[0] + cpu_state_high.XMM[cpu_rm].b[0] + 1;
        tmp[1]  = cpu_state_high.XMM[cpu_reg].b[1] + cpu_state_high.XMM[cpu_rm].b[1] + 1;
        tmp[2]  = cpu_state_high.XMM[cpu_reg].b[2] + cpu_state_high.XMM[cpu_rm].b[2] + 1;
        tmp[3]  = cpu_state_high.XMM[cpu_reg].b[3] + cpu_state_high.XMM[cpu_rm].b[3] + 1;
        tmp[4]  = cpu_state_high.XMM[cpu_reg].b[4] + cpu_state_high.XMM[cpu_rm].b[4] + 1;
        tmp[5]  = cpu_state_high.XMM[cpu_reg].b[5] + cpu_state_high.XMM[cpu_rm].b[5] + 1;
        tmp[6]  = cpu_state_high.XMM[cpu_reg].b[6] + cpu_state_high.XMM[cpu_rm].b[6] + 1;
        tmp[7]  = cpu_state_high.XMM[cpu_reg].b[7] + cpu_state_high.XMM[cpu_rm].b[7] + 1;
        tmp[8]  = cpu_state_high.XMM[cpu_reg].b[8] + cpu_state_high.XMM[cpu_rm].b[8] + 1;
        tmp[9]  = cpu_state_high.XMM[cpu_reg].b[9] + cpu_state_high.XMM[cpu_rm].b[9] + 1;
        tmp[10] = cpu_state_high.XMM[cpu_reg].b[10] + cpu_state_high.XMM[cpu_rm].b[10] + 1;
        tmp[11] = cpu_state_high.XMM[cpu_reg].b[11] + cpu_state_high.XMM[cpu_rm].b[11] + 1;
        tmp[12] = cpu_state_high.XMM[cpu_reg].b[12] + cpu_state_high.XMM[cpu_rm].b[12] + 1;
        tmp[13] = cpu_state_high.XMM[cpu_reg].b[13] + cpu_state_high.XMM[cpu_rm].b[13] + 1;
        tmp[14] = cpu_state_high.XMM[cpu_reg].b[14] + cpu_state_high.XMM[cpu_rm].b[14] + 1;
        tmp[15] = cpu_state_high.XMM[cpu_reg].b[15] + cpu_state_high.XMM[cpu_rm].b[15] + 1;

        cpu_state_high.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state_high.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state_high.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state_high.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state_high.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state_high.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state_high.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state_high.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state_high.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;

        tmp[0]  = cpu_state_high.XMM[cpu_reg].b[0] + src[0] + 1;
        tmp[1]  = cpu_state_high.XMM[cpu_reg].b[1] + src[1] + 1;
        tmp[2]  = cpu_state_high.XMM[cpu_reg].b[2] + src[2] + 1;
        tmp[3]  = cpu_state_high.XMM[cpu_reg].b[3] + src[3] + 1;
        tmp[4]  = cpu_state_high.XMM[cpu_reg].b[4] + src[4] + 1;
        tmp[5]  = cpu_state_high.XMM[cpu_reg].b[5] + src[5] + 1;
        tmp[6]  = cpu_state_high.XMM[cpu_reg].b[6] + src[6] + 1;
        tmp[7]  = cpu_state_high.XMM[cpu_reg].b[7] + src[7] + 1;
        tmp[8]  = cpu_state_high.XMM[cpu_reg].b[8] + src[8] + 1;
        tmp[9]  = cpu_state_high.XMM[cpu_reg].b[9] + src[9] + 1;
        tmp[10] = cpu_state_high.XMM[cpu_reg].b[10] + src[10] + 1;
        tmp[11] = cpu_state_high.XMM[cpu_reg].b[11] + src[11] + 1;
        tmp[12] = cpu_state_high.XMM[cpu_reg].b[12] + src[12] + 1;
        tmp[13] = cpu_state_high.XMM[cpu_reg].b[13] + src[13] + 1;
        tmp[14] = cpu_state_high.XMM[cpu_reg].b[14] + src[14] + 1;
        tmp[15] = cpu_state_high.XMM[cpu_reg].b[15] + src[15] + 1;

        cpu_state_high.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state_high.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state_high.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state_high.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state_high.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state_high.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state_high.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state_high.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state_high.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
    }

    return 0;
}

static int
opPAVGB_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPAVGB_xmm_xmm_a16(fetchdat);
    else
        return opPAVGB_mm_mm_a16(fetchdat);
}

static int
opPAVGB_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPAVGB_xmm_xmm_a32(fetchdat);
    else
        return opPAVGB_mm_mm_a32(fetchdat);
}

static int
opPAVGW_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    uint32_t tmp[4];

    tmp[0] = dst->w[0] + src.w[0] + 1;
    tmp[1] = dst->w[1] + src.w[1] + 1;
    tmp[2] = dst->w[2] + src.w[2] + 1;
    tmp[3] = dst->w[3] + src.w[3] + 1;

    dst->w[0] = (uint16_t) (tmp[0] >> 1);
    dst->w[1] = (uint16_t) (tmp[1] >> 1);
    dst->w[2] = (uint16_t) (tmp[2] >> 1);
    dst->w[3] = (uint16_t) (tmp[3] >> 1);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPAVGW_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    uint32_t tmp[4];

    tmp[0] = dst->w[0] + src.w[0] + 1;
    tmp[1] = dst->w[1] + src.w[1] + 1;
    tmp[2] = dst->w[2] + src.w[2] + 1;
    tmp[3] = dst->w[3] + src.w[3] + 1;

    dst->w[0] = (uint16_t) (tmp[0] >> 1);
    dst->w[1] = (uint16_t) (tmp[1] >> 1);
    dst->w[2] = (uint16_t) (tmp[2] >> 1);
    dst->w[3] = (uint16_t) (tmp[3] >> 1);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPAVGW_xmm_xmm_a16(uint32_t fetchdat)
{
    uint32_t tmp[8];
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        tmp[0] = cpu_state_high.XMM[cpu_reg].w[0] + cpu_state_high.XMM[cpu_rm].w[0] + 1;
        tmp[1] = cpu_state_high.XMM[cpu_reg].w[1] + cpu_state_high.XMM[cpu_rm].w[1] + 1;
        tmp[2] = cpu_state_high.XMM[cpu_reg].w[2] + cpu_state_high.XMM[cpu_rm].w[2] + 1;
        tmp[3] = cpu_state_high.XMM[cpu_reg].w[3] + cpu_state_high.XMM[cpu_rm].w[3] + 1;
        tmp[4] = cpu_state_high.XMM[cpu_reg].w[4] + cpu_state_high.XMM[cpu_rm].w[4] + 1;
        tmp[5] = cpu_state_high.XMM[cpu_reg].w[5] + cpu_state_high.XMM[cpu_rm].w[5] + 1;
        tmp[6] = cpu_state_high.XMM[cpu_reg].w[6] + cpu_state_high.XMM[cpu_rm].w[6] + 1;
        tmp[7] = cpu_state_high.XMM[cpu_reg].w[7] + cpu_state_high.XMM[cpu_rm].w[7] + 1;

        cpu_state_high.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
        CLOCK_CYCLES(1);
    } else {
        uint16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;

        tmp[0] = cpu_state_high.XMM[cpu_reg].w[0] + src[0] + 1;
        tmp[1] = cpu_state_high.XMM[cpu_reg].w[1] + src[1] + 1;
        tmp[2] = cpu_state_high.XMM[cpu_reg].w[2] + src[2] + 1;
        tmp[3] = cpu_state_high.XMM[cpu_reg].w[3] + src[3] + 1;
        tmp[4] = cpu_state_high.XMM[cpu_reg].w[4] + src[4] + 1;
        tmp[5] = cpu_state_high.XMM[cpu_reg].w[5] + src[5] + 1;
        tmp[6] = cpu_state_high.XMM[cpu_reg].w[6] + src[6] + 1;
        tmp[7] = cpu_state_high.XMM[cpu_reg].w[7] + src[7] + 1;

        cpu_state_high.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
    }

    return 0;
}

static int
opPAVGW_xmm_xmm_a32(uint32_t fetchdat)
{
    uint32_t tmp[8];
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        tmp[0] = cpu_state_high.XMM[cpu_reg].w[0] + cpu_state_high.XMM[cpu_rm].w[0] + 1;
        tmp[1] = cpu_state_high.XMM[cpu_reg].w[1] + cpu_state_high.XMM[cpu_rm].w[1] + 1;
        tmp[2] = cpu_state_high.XMM[cpu_reg].w[2] + cpu_state_high.XMM[cpu_rm].w[2] + 1;
        tmp[3] = cpu_state_high.XMM[cpu_reg].w[3] + cpu_state_high.XMM[cpu_rm].w[3] + 1;
        tmp[4] = cpu_state_high.XMM[cpu_reg].w[4] + cpu_state_high.XMM[cpu_rm].w[4] + 1;
        tmp[5] = cpu_state_high.XMM[cpu_reg].w[5] + cpu_state_high.XMM[cpu_rm].w[5] + 1;
        tmp[6] = cpu_state_high.XMM[cpu_reg].w[6] + cpu_state_high.XMM[cpu_rm].w[6] + 1;
        tmp[7] = cpu_state_high.XMM[cpu_reg].w[7] + cpu_state_high.XMM[cpu_rm].w[7] + 1;

        cpu_state_high.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
        CLOCK_CYCLES(1);
    } else {
        uint16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;

        tmp[0] = cpu_state_high.XMM[cpu_reg].w[0] + src[0] + 1;
        tmp[1] = cpu_state_high.XMM[cpu_reg].w[1] + src[1] + 1;
        tmp[2] = cpu_state_high.XMM[cpu_reg].w[2] + src[2] + 1;
        tmp[3] = cpu_state_high.XMM[cpu_reg].w[3] + src[3] + 1;
        tmp[4] = cpu_state_high.XMM[cpu_reg].w[4] + src[4] + 1;
        tmp[5] = cpu_state_high.XMM[cpu_reg].w[5] + src[5] + 1;
        tmp[6] = cpu_state_high.XMM[cpu_reg].w[6] + src[6] + 1;
        tmp[7] = cpu_state_high.XMM[cpu_reg].w[7] + src[7] + 1;

        cpu_state_high.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state_high.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state_high.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state_high.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state_high.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state_high.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state_high.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state_high.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
    }

    return 0;
}

static int
opPAVGW_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPAVGW_xmm_xmm_a16(fetchdat);
    else
        return opPAVGW_mm_mm_a16(fetchdat);
}

static int
opPAVGW_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPAVGW_xmm_xmm_a32(fetchdat);
    else
        return opPAVGW_mm_mm_a32(fetchdat);
}

static int
opPMULHUW_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->w[0] = ((uint32_t)dst->w[0] * (uint32_t)src.w[0]) >> 16;
    dst->w[1] = ((uint32_t)dst->w[1] * (uint32_t)src.w[1]) >> 16;
    dst->w[2] = ((uint32_t)dst->w[2] * (uint32_t)src.w[2]) >> 16;
    dst->w[3] = ((uint32_t)dst->w[3] * (uint32_t)src.w[3]) >> 16;

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMULHUW_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->w[0] = ((uint32_t)dst->w[0] * (uint32_t)src.w[0]) >> 16;
    dst->w[1] = ((uint32_t)dst->w[1] * (uint32_t)src.w[1]) >> 16;
    dst->w[2] = ((uint32_t)dst->w[2] * (uint32_t)src.w[2]) >> 16;
    dst->w[3] = ((uint32_t)dst->w[3] * (uint32_t)src.w[3]) >> 16;

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMULHUW_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[0] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[0]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[1] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[1]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[2] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[2]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[3] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[3]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[4] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[4]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[5] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[5]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[6] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[6]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[7] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[7]) >> 16;
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[0] * src[0]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[1] * src[1]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[2] * src[2]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[3] * src[3]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[4] * src[4]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[5] * src[5]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[6] * src[6]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[7] * src[7]) >> 16;
    }

    return 0;
}

static int
opPMULHUW_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[0] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[0]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[1] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[1]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[2] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[2]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[3] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[3]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[4] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[4]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[5] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[5]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[6] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[6]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[7] * (uint32_t) cpu_state_high.XMM[cpu_rm].w[7]) >> 16;
        CLOCK_CYCLES(1);
    } else {
        uint32_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[0] * src[0]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[1] * src[1]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[2] * src[2]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[3] * src[3]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[4] * src[4]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[5] * src[5]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[6] * src[6]) >> 16;
        cpu_state_high.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state_high.XMM[cpu_reg].w[7] * src[7]) >> 16;
    }

    return 0;
}

static int
opPMULHUW_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMULHUW_xmm_xmm_a16(fetchdat);
    else
        return opPMULHUW_mm_mm_a16(fetchdat);
}

static int
opPMULHUW_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMULHUW_xmm_xmm_a32(fetchdat);
    else
        return opPMULHUW_mm_mm_a32(fetchdat);
}

static int
opPMINSW_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->sw[0] = fmin(dst->sw[0], src.sw[0]);
    dst->sw[1] = fmin(dst->sw[1], src.sw[1]);
    dst->sw[2] = fmin(dst->sw[2], src.sw[2]);
    dst->sw[3] = fmin(dst->sw[3], src.sw[3]);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMINSW_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->sw[0] = fmin(dst->sw[0], src.sw[0]);
    dst->sw[1] = fmin(dst->sw[1], src.sw[1]);
    dst->sw[2] = fmin(dst->sw[2], src.sw[2]);
    dst->sw[3] = fmin(dst->sw[3], src.sw[3]);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMINSW_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sw[0] = fmin(cpu_state_high.XMM[cpu_reg].sw[0], cpu_state_high.XMM[cpu_rm].sw[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmin(cpu_state_high.XMM[cpu_reg].sw[1], cpu_state_high.XMM[cpu_rm].sw[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmin(cpu_state_high.XMM[cpu_reg].sw[2], cpu_state_high.XMM[cpu_rm].sw[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmin(cpu_state_high.XMM[cpu_reg].sw[3], cpu_state_high.XMM[cpu_rm].sw[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmin(cpu_state_high.XMM[cpu_reg].sw[4], cpu_state_high.XMM[cpu_rm].sw[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmin(cpu_state_high.XMM[cpu_reg].sw[5], cpu_state_high.XMM[cpu_rm].sw[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmin(cpu_state_high.XMM[cpu_reg].sw[6], cpu_state_high.XMM[cpu_rm].sw[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmin(cpu_state_high.XMM[cpu_reg].sw[7], cpu_state_high.XMM[cpu_rm].sw[7]);
        CLOCK_CYCLES(1);
    } else {
        int16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].sw[0] = fmin(cpu_state_high.XMM[cpu_reg].sw[0], src[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmin(cpu_state_high.XMM[cpu_reg].sw[1], src[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmin(cpu_state_high.XMM[cpu_reg].sw[2], src[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmin(cpu_state_high.XMM[cpu_reg].sw[3], src[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmin(cpu_state_high.XMM[cpu_reg].sw[4], src[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmin(cpu_state_high.XMM[cpu_reg].sw[5], src[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmin(cpu_state_high.XMM[cpu_reg].sw[6], src[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmin(cpu_state_high.XMM[cpu_reg].sw[7], src[7]);
    }

    return 0;
}

static int
opPMINSW_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sw[0] = fmin(cpu_state_high.XMM[cpu_reg].sw[0], cpu_state_high.XMM[cpu_rm].sw[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmin(cpu_state_high.XMM[cpu_reg].sw[1], cpu_state_high.XMM[cpu_rm].sw[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmin(cpu_state_high.XMM[cpu_reg].sw[2], cpu_state_high.XMM[cpu_rm].sw[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmin(cpu_state_high.XMM[cpu_reg].sw[3], cpu_state_high.XMM[cpu_rm].sw[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmin(cpu_state_high.XMM[cpu_reg].sw[4], cpu_state_high.XMM[cpu_rm].sw[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmin(cpu_state_high.XMM[cpu_reg].sw[5], cpu_state_high.XMM[cpu_rm].sw[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmin(cpu_state_high.XMM[cpu_reg].sw[6], cpu_state_high.XMM[cpu_rm].sw[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmin(cpu_state_high.XMM[cpu_reg].sw[7], cpu_state_high.XMM[cpu_rm].sw[7]);
        CLOCK_CYCLES(1);
    } else {
        int16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].sw[0] = fmin(cpu_state_high.XMM[cpu_reg].sw[0], src[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmin(cpu_state_high.XMM[cpu_reg].sw[1], src[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmin(cpu_state_high.XMM[cpu_reg].sw[2], src[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmin(cpu_state_high.XMM[cpu_reg].sw[3], src[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmin(cpu_state_high.XMM[cpu_reg].sw[4], src[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmin(cpu_state_high.XMM[cpu_reg].sw[5], src[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmin(cpu_state_high.XMM[cpu_reg].sw[6], src[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmin(cpu_state_high.XMM[cpu_reg].sw[7], src[7]);
    }

    return 0;
}

static int
opPMINSW_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMINSW_xmm_xmm_a16(fetchdat);
    else
        return opPMINSW_mm_mm_a16(fetchdat);
}

static int
opPMINSW_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMINSW_xmm_xmm_a32(fetchdat);
    else
        return opPMINSW_mm_mm_a32(fetchdat);
}

static int
opPMAXSW_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->sw[0] = fmax(dst->sw[0], src.sw[0]);
    dst->sw[1] = fmax(dst->sw[1], src.sw[1]);
    dst->sw[2] = fmax(dst->sw[2], src.sw[2]);
    dst->sw[3] = fmax(dst->sw[3], src.sw[3]);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMAXSW_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    dst->sw[0] = fmax(dst->sw[0], src.sw[0]);
    dst->sw[1] = fmax(dst->sw[1], src.sw[1]);
    dst->sw[2] = fmax(dst->sw[2], src.sw[2]);
    dst->sw[3] = fmax(dst->sw[3], src.sw[3]);

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMAXSW_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sw[0] = fmax(cpu_state_high.XMM[cpu_reg].sw[0], cpu_state_high.XMM[cpu_rm].sw[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmax(cpu_state_high.XMM[cpu_reg].sw[1], cpu_state_high.XMM[cpu_rm].sw[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmax(cpu_state_high.XMM[cpu_reg].sw[2], cpu_state_high.XMM[cpu_rm].sw[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmax(cpu_state_high.XMM[cpu_reg].sw[3], cpu_state_high.XMM[cpu_rm].sw[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmax(cpu_state_high.XMM[cpu_reg].sw[4], cpu_state_high.XMM[cpu_rm].sw[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmax(cpu_state_high.XMM[cpu_reg].sw[5], cpu_state_high.XMM[cpu_rm].sw[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmax(cpu_state_high.XMM[cpu_reg].sw[6], cpu_state_high.XMM[cpu_rm].sw[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmax(cpu_state_high.XMM[cpu_reg].sw[7], cpu_state_high.XMM[cpu_rm].sw[7]);
        CLOCK_CYCLES(1);
    } else {
        int16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].sw[0] = fmax(cpu_state_high.XMM[cpu_reg].sw[0], src[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmax(cpu_state_high.XMM[cpu_reg].sw[1], src[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmax(cpu_state_high.XMM[cpu_reg].sw[2], src[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmax(cpu_state_high.XMM[cpu_reg].sw[3], src[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmax(cpu_state_high.XMM[cpu_reg].sw[4], src[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmax(cpu_state_high.XMM[cpu_reg].sw[5], src[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmax(cpu_state_high.XMM[cpu_reg].sw[6], src[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmax(cpu_state_high.XMM[cpu_reg].sw[7], src[7]);
    }

    return 0;
}

static int
opPMAXSW_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sw[0] = fmax(cpu_state_high.XMM[cpu_reg].sw[0], cpu_state_high.XMM[cpu_rm].sw[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmax(cpu_state_high.XMM[cpu_reg].sw[1], cpu_state_high.XMM[cpu_rm].sw[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmax(cpu_state_high.XMM[cpu_reg].sw[2], cpu_state_high.XMM[cpu_rm].sw[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmax(cpu_state_high.XMM[cpu_reg].sw[3], cpu_state_high.XMM[cpu_rm].sw[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmax(cpu_state_high.XMM[cpu_reg].sw[4], cpu_state_high.XMM[cpu_rm].sw[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmax(cpu_state_high.XMM[cpu_reg].sw[5], cpu_state_high.XMM[cpu_rm].sw[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmax(cpu_state_high.XMM[cpu_reg].sw[6], cpu_state_high.XMM[cpu_rm].sw[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmax(cpu_state_high.XMM[cpu_reg].sw[7], cpu_state_high.XMM[cpu_rm].sw[7]);
        CLOCK_CYCLES(1);
    } else {
        int16_t src[8];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemw(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemw(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemw(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemw(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemw(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemw(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemw(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemw(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        cpu_state_high.XMM[cpu_reg].sw[0] = fmax(cpu_state_high.XMM[cpu_reg].sw[0], src[0]);
        cpu_state_high.XMM[cpu_reg].sw[1] = fmax(cpu_state_high.XMM[cpu_reg].sw[1], src[1]);
        cpu_state_high.XMM[cpu_reg].sw[2] = fmax(cpu_state_high.XMM[cpu_reg].sw[2], src[2]);
        cpu_state_high.XMM[cpu_reg].sw[3] = fmax(cpu_state_high.XMM[cpu_reg].sw[3], src[3]);
        cpu_state_high.XMM[cpu_reg].sw[4] = fmax(cpu_state_high.XMM[cpu_reg].sw[4], src[4]);
        cpu_state_high.XMM[cpu_reg].sw[5] = fmax(cpu_state_high.XMM[cpu_reg].sw[5], src[5]);
        cpu_state_high.XMM[cpu_reg].sw[6] = fmax(cpu_state_high.XMM[cpu_reg].sw[6], src[6]);
        cpu_state_high.XMM[cpu_reg].sw[7] = fmax(cpu_state_high.XMM[cpu_reg].sw[7], src[7]);
    }

    return 0;
}

static int
opPMAXSW_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMAXSW_xmm_xmm_a16(fetchdat);
    else
        return opPMAXSW_mm_mm_a16(fetchdat);
}

static int
opPMAXSW_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPMAXSW_xmm_xmm_a32(fetchdat);
    else
        return opPMAXSW_mm_mm_a32(fetchdat);
}

static int
opPSADBW_mm_mm_a16(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;
    MMX_REG  temp;

    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    temp.b[0] = abs((int8_t)(dst->b[0] - src.b[0]));
    temp.b[1] = abs((int8_t)(dst->b[1] - src.b[1]));
    temp.b[2] = abs((int8_t)(dst->b[2] - src.b[2]));
    temp.b[3] = abs((int8_t)(dst->b[3] - src.b[3]));
    temp.b[4] = abs((int8_t)(dst->b[4] - src.b[4]));
    temp.b[5] = abs((int8_t)(dst->b[5] - src.b[5]));
    temp.b[6] = abs((int8_t)(dst->b[6] - src.b[6]));
    temp.b[7] = abs((int8_t)(dst->b[7] - src.b[7]));

    uint16_t result = 0;
    for (int i = 0; i < 8; i++) {
        result += temp.b[i];
    }
    dst->w[0] = result;
    dst->w[1] = 0;
    dst->w[2] = 0;
    dst->w[3] = 0;

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPSADBW_mm_mm_a32(uint32_t fetchdat)
{
    MMX_REG  src;
    MMX_REG *dst;
    MMX_REG  temp;

    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);

    MMX_GETSRC();

    temp.b[0] = abs((int8_t)(dst->b[0] - src.b[0]));
    temp.b[1] = abs((int8_t)(dst->b[1] - src.b[1]));
    temp.b[2] = abs((int8_t)(dst->b[2] - src.b[2]));
    temp.b[3] = abs((int8_t)(dst->b[3] - src.b[3]));
    temp.b[4] = abs((int8_t)(dst->b[4] - src.b[4]));
    temp.b[5] = abs((int8_t)(dst->b[5] - src.b[5]));
    temp.b[6] = abs((int8_t)(dst->b[6] - src.b[6]));
    temp.b[7] = abs((int8_t)(dst->b[7] - src.b[7]));

    uint16_t result = 0;
    for (int i = 0; i < 8; i++) {
        result += temp.b[i];
    }
    dst->w[0] = result;
    dst->w[1] = 0;
    dst->w[2] = 0;
    dst->w[3] = 0;

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPSADBW_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        uint8_t temp[16];
        temp[0]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[0] - cpu_state_high.XMM[cpu_rm].b[0]));
        temp[1]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[1] - cpu_state_high.XMM[cpu_rm].b[1]));
        temp[2]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[2] - cpu_state_high.XMM[cpu_rm].b[2]));
        temp[3]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[3] - cpu_state_high.XMM[cpu_rm].b[3]));
        temp[4]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[4] - cpu_state_high.XMM[cpu_rm].b[4]));
        temp[5]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[5] - cpu_state_high.XMM[cpu_rm].b[5]));
        temp[6]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[6] - cpu_state_high.XMM[cpu_rm].b[6]));
        temp[7]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[7] - cpu_state_high.XMM[cpu_rm].b[7]));
        temp[8]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[8] - cpu_state_high.XMM[cpu_rm].b[8]));
        temp[9]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[9] - cpu_state_high.XMM[cpu_rm].b[9]));
        temp[10]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[10] - cpu_state_high.XMM[cpu_rm].b[10]));
        temp[11]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[11] - cpu_state_high.XMM[cpu_rm].b[11]));
        temp[12]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[12] - cpu_state_high.XMM[cpu_rm].b[12]));
        temp[13]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[13] - cpu_state_high.XMM[cpu_rm].b[13]));
        temp[14]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[14] - cpu_state_high.XMM[cpu_rm].b[14]));
        temp[15]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[15] - cpu_state_high.XMM[cpu_rm].b[15]));
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state_high.XMM[cpu_reg].w[0] = result;
        cpu_state_high.XMM[cpu_reg].w[1] = 0;
        cpu_state_high.XMM[cpu_reg].w[2] = 0;
        cpu_state_high.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state_high.XMM[cpu_reg].w[4] = result;
        cpu_state_high.XMM[cpu_reg].w[5] = 0;
        cpu_state_high.XMM[cpu_reg].w[6] = 0;
        cpu_state_high.XMM[cpu_reg].w[7] = 0;
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];
        uint8_t temp[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        temp[0]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[0] - src[0]));
        temp[1]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[1] - src[1]));
        temp[2]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[2] - src[2]));
        temp[3]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[3] - src[3]));
        temp[4]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[4] - src[4]));
        temp[5]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[5] - src[5]));
        temp[6]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[6] - src[6]));
        temp[7]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[7] - src[7]));
        temp[8]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[8] - src[8]));
        temp[9]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[9] - src[9]));
        temp[10]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[10] - src[10]));
        temp[11]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[11] - src[11]));
        temp[12]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[12] - src[12]));
        temp[13]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[13] - src[13]));
        temp[14]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[14] - src[14]));
        temp[15]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[15] - src[15]));
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state_high.XMM[cpu_reg].w[0] = result;
        cpu_state_high.XMM[cpu_reg].w[1] = 0;
        cpu_state_high.XMM[cpu_reg].w[2] = 0;
        cpu_state_high.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state_high.XMM[cpu_reg].w[4] = result;
        cpu_state_high.XMM[cpu_reg].w[5] = 0;
        cpu_state_high.XMM[cpu_reg].w[6] = 0;
        cpu_state_high.XMM[cpu_reg].w[7] = 0;
    }

    return 0;
}

static int
opPSADBW_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        uint8_t temp[16];
        temp[0]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[0] - cpu_state_high.XMM[cpu_rm].b[0]));
        temp[1]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[1] - cpu_state_high.XMM[cpu_rm].b[1]));
        temp[2]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[2] - cpu_state_high.XMM[cpu_rm].b[2]));
        temp[3]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[3] - cpu_state_high.XMM[cpu_rm].b[3]));
        temp[4]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[4] - cpu_state_high.XMM[cpu_rm].b[4]));
        temp[5]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[5] - cpu_state_high.XMM[cpu_rm].b[5]));
        temp[6]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[6] - cpu_state_high.XMM[cpu_rm].b[6]));
        temp[7]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[7] - cpu_state_high.XMM[cpu_rm].b[7]));
        temp[8]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[8] - cpu_state_high.XMM[cpu_rm].b[8]));
        temp[9]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[9] - cpu_state_high.XMM[cpu_rm].b[9]));
        temp[10]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[10] - cpu_state_high.XMM[cpu_rm].b[10]));
        temp[11]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[11] - cpu_state_high.XMM[cpu_rm].b[11]));
        temp[12]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[12] - cpu_state_high.XMM[cpu_rm].b[12]));
        temp[13]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[13] - cpu_state_high.XMM[cpu_rm].b[13]));
        temp[14]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[14] - cpu_state_high.XMM[cpu_rm].b[14]));
        temp[15]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[15] - cpu_state_high.XMM[cpu_rm].b[15]));
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state_high.XMM[cpu_reg].w[0] = result;
        cpu_state_high.XMM[cpu_reg].w[1] = 0;
        cpu_state_high.XMM[cpu_reg].w[2] = 0;
        cpu_state_high.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state_high.XMM[cpu_reg].w[4] = result;
        cpu_state_high.XMM[cpu_reg].w[5] = 0;
        cpu_state_high.XMM[cpu_reg].w[6] = 0;
        cpu_state_high.XMM[cpu_reg].w[7] = 0;
        CLOCK_CYCLES(1);
    } else {
        uint8_t src[16];
        uint8_t temp[16];

        SEG_CHECK_READ(cpu_state.ea_seg);
        src[0] = readmemb(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        src[1] = readmemb(easeg, cpu_state.eaaddr + 1);
        if (cpu_state.abrt)
            return 1;
        src[2] = readmemb(easeg, cpu_state.eaaddr + 2);
        if (cpu_state.abrt)
            return 1;
        src[3] = readmemb(easeg, cpu_state.eaaddr + 3);
        if (cpu_state.abrt)
            return 1;
        src[4] = readmemb(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        src[5] = readmemb(easeg, cpu_state.eaaddr + 5);
        if (cpu_state.abrt)
            return 1;
        src[6] = readmemb(easeg, cpu_state.eaaddr + 6);
        if (cpu_state.abrt)
            return 1;
        src[7] = readmemb(easeg, cpu_state.eaaddr + 7);
        if (cpu_state.abrt)
            return 1;
        src[8] = readmemb(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        src[9] = readmemb(easeg, cpu_state.eaaddr + 9);
        if (cpu_state.abrt)
            return 1;
        src[10] = readmemb(easeg, cpu_state.eaaddr + 10);
        if (cpu_state.abrt)
            return 1;
        src[11] = readmemb(easeg, cpu_state.eaaddr + 11);
        if (cpu_state.abrt)
            return 1;
        src[12] = readmemb(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        src[13] = readmemb(easeg, cpu_state.eaaddr + 13);
        if (cpu_state.abrt)
            return 1;
        src[14] = readmemb(easeg, cpu_state.eaaddr + 14);
        if (cpu_state.abrt)
            return 1;
        src[15] = readmemb(easeg, cpu_state.eaaddr + 15);
        if (cpu_state.abrt)
            return 1;
        temp[0]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[0] - src[0]));
        temp[1]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[1] - src[1]));
        temp[2]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[2] - src[2]));
        temp[3]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[3] - src[3]));
        temp[4]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[4] - src[4]));
        temp[5]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[5] - src[5]));
        temp[6]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[6] - src[6]));
        temp[7]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[7] - src[7]));
        temp[8]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[8] - src[8]));
        temp[9]         = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[9] - src[9]));
        temp[10]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[10] - src[10]));
        temp[11]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[11] - src[11]));
        temp[12]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[12] - src[12]));
        temp[13]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[13] - src[13]));
        temp[14]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[14] - src[14]));
        temp[15]        = abs((int8_t)(cpu_state_high.XMM[cpu_reg].b[15] - src[15]));
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state_high.XMM[cpu_reg].w[0] = result;
        cpu_state_high.XMM[cpu_reg].w[1] = 0;
        cpu_state_high.XMM[cpu_reg].w[2] = 0;
        cpu_state_high.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state_high.XMM[cpu_reg].w[4] = result;
        cpu_state_high.XMM[cpu_reg].w[5] = 0;
        cpu_state_high.XMM[cpu_reg].w[6] = 0;
        cpu_state_high.XMM[cpu_reg].w[7] = 0;
    }

    return 0;
}

static int
opPSADBW_a16(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPSADBW_xmm_xmm_a16(fetchdat);
    else
        return opPSADBW_mm_mm_a16(fetchdat);
}

static int
opPSADBW_a32(uint32_t fetchdat)
{
    if (sse_xmm)
        return opPSADBW_xmm_xmm_a32(fetchdat);
    else
        return opPSADBW_mm_mm_a32(fetchdat);
}
