/* SPDX-License-Identifier: GPL-2.0-or-later */
static int
opCVTPI2PD_xmm_mm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    MMX_REG src;
    MMX_ENTER();
    fetch_ea_16(fetchdat);
    MMX_GETSRC();

    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    cpu_state_high.XMM[cpu_reg].d2[0] = src.sl[0];
    cpu_state_high.XMM[cpu_reg].d2[1] = src.sl[1];
    fesetround(FE_TONEAREST);
    //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
    //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
    CLOCK_CYCLES(1);
    return 0;
}

static int
opCVTPI2PD_xmm_mm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    MMX_REG src;
    MMX_ENTER();
    fetch_ea_32(fetchdat);
    MMX_GETSRC();

    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    cpu_state_high.XMM[cpu_reg].d2[0] = src.sl[0];
    cpu_state_high.XMM[cpu_reg].d2[1] = src.sl[1];
    fesetround(FE_TONEAREST);
    //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
    //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
    CLOCK_CYCLES(1);
    return 0;
}

static int
opCVTSI2SD_xmm_l_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = (int32_t)getr32(cpu_rm);
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSI2SD_xmm_l_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = (int32_t)getr32(cpu_rm);
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTTPD2PI_mm_xmm_a16(uint32_t fetchdat)
{
    SSE_REG src;
    MMX_REG *dst;
    MMX_ENTER();
    fetch_ea_16(fetchdat);
    
    dst = MMX_GETREGP(cpu_reg);
    SSE_GETSRC();
    dst->sl[0] = trunc(src.d2[0]);
    dst->sl[1] = trunc(src.d2[1]);
    MMX_SETEXP(cpu_reg);
    return 0;
}

static int
opCVTTPD2PI_mm_xmm_a32(uint32_t fetchdat)
{
    SSE_REG src;
    MMX_REG *dst;
    MMX_ENTER();
    fetch_ea_32(fetchdat);

    dst = MMX_GETREGP(cpu_reg);
    SSE_GETSRC();
    dst->sl[0] = trunc(src.d2[0]);
    dst->sl[1] = trunc(src.d2[1]);
    MMX_SETEXP(cpu_reg);
    return 0;
}

static int
opCVTTSD2SI_l_xmm_a16(uint32_t fetchdat)
{
    SSE_REG src;
    int32_t result;
    fetch_ea_16(fetchdat);
    SSE_GETSRC();
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    result = trunc(src.d2[0]);
    setr32(cpu_reg, result);
    fesetround(FE_TONEAREST);
    return 0;
}

static int
opCVTTSD2SI_l_xmm_a32(uint32_t fetchdat)
{
    SSE_REG src;
    int32_t result;
    fetch_ea_32(fetchdat);
    SSE_GETSRC();
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    result = trunc(src.d2[0]);
    setr32(cpu_reg, result);
    fesetround(FE_TONEAREST);
}

static int
opCVTSD2SI_l_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        setr32(cpu_reg, cpu_state_high.XMM[cpu_rm].d2[0]);
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        double dst_real;
        dst_real = *(double *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        setr32(cpu_reg, dst_real);
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSD2SI_l_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        setr32(cpu_reg, cpu_state_high.XMM[cpu_rm].d2[0]);
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        double dst_real;
        dst_real = *(double *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        setr32(cpu_reg, dst_real);
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPD2PS_mm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = cpu_state_high.XMM[cpu_rm].d2[1];
        cpu_state_high.XMM[cpu_reg].l[2] = 0;
        cpu_state_high.XMM[cpu_reg].l[3] = 0;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0]       = *(double *) &dst[0];
        dst_real[1]       = *(double *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = dst_real[1];
        cpu_state_high.XMM[cpu_reg].l[2] = 0;
        cpu_state_high.XMM[cpu_reg].l[3] = 0;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPD2PS_mm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = cpu_state_high.XMM[cpu_rm].d2[1];
        cpu_state_high.XMM[cpu_reg].l[2] = 0;
        cpu_state_high.XMM[cpu_reg].l[3] = 0;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0]       = *(double *) &dst[0];
        dst_real[1]       = *(double *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = dst_real[1];
        cpu_state_high.XMM[cpu_reg].l[2] = 0;
        cpu_state_high.XMM[cpu_reg].l[3] = 0;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPS2PD_mm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    if ((cpu_features & CPU_FEATURE_SSE2) && sse_xmm)
        return opCVTPD2PS_mm_xmm_a16(fetchdat);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].f2[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = cpu_state_high.XMM[cpu_rm].f2[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        float dst_real[2];
        dst_real[0]       = *(float *) &dst[0];
        dst_real[1]       = *(float *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = dst_real[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPS2PD_mm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    if ((cpu_features & CPU_FEATURE_SSE2) && sse_xmm)
        return opCVTPD2PS_mm_xmm_a32(fetchdat);
    fetch_ea_32(fetchdat);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].f2[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = cpu_state_high.XMM[cpu_rm].f2[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        float dst_real[2];
        dst_real[0]       = *(float *) &dst[0];
        dst_real[1]       = *(float *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = dst_real[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSS2SD_mm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].f2[0];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float dst_real;
        dst_real          = *(float *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst_real;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSS2SD_mm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].f2[0];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        float dst_real;
        dst_real          = *(float *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst_real;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSD2SS_mm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        double dst_real;
        dst_real          = *(double *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst_real;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTSD2SS_mm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst;

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        double dst_real;
        dst_real          = *(double *) &dst;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst_real;
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPS2DQ_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_REG src;
    fetch_ea_16(fetchdat);
    SSE_GETSRC();
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    cpu_state_high.XMM[cpu_reg].sl[0] = src.f2[0];
    cpu_state_high.XMM[cpu_reg].sl[1] = src.f2[1];
    cpu_state_high.XMM[cpu_reg].sl[2] = src.f2[2];
    cpu_state_high.XMM[cpu_reg].sl[3] = src.f2[3];
    fesetround(FE_TONEAREST);
    return 0;
}

static int
opCVTPS2DQ_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_REG src;
    fetch_ea_32(fetchdat);
    SSE_GETSRC();
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    cpu_state_high.XMM[cpu_reg].sl[0] = src.f2[0];
    cpu_state_high.XMM[cpu_reg].sl[1] = src.f2[1];
    cpu_state_high.XMM[cpu_reg].sl[2] = src.f2[2];
    cpu_state_high.XMM[cpu_reg].sl[3] = src.f2[3];
    fesetround(FE_TONEAREST);
    return 0;
}

static int
opCVTDQ2PS_xmm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    if ((cpu_features & CPU_FEATURE_SSE2) && sse_xmm)
        return opCVTPS2DQ_xmm_xmm_a16(fetchdat);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].sl[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = cpu_state_high.XMM[cpu_rm].sl[1];
        cpu_state_high.XMM[cpu_reg].f2[2] = cpu_state_high.XMM[cpu_rm].sl[2];
        cpu_state_high.XMM[cpu_reg].f2[3] = cpu_state_high.XMM[cpu_rm].sl[3];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[2]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[3]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        dst[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        dst[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = dst[1];
        cpu_state_high.XMM[cpu_reg].f2[2] = dst[2];
        cpu_state_high.XMM[cpu_reg].f2[3] = dst[3];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[2]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[3]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTDQ2PS_xmm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    if ((cpu_features & CPU_FEATURE_SSE2) && sse_xmm)
        return opCVTPS2DQ_xmm_xmm_a32(fetchdat);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = cpu_state_high.XMM[cpu_rm].sl[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = cpu_state_high.XMM[cpu_rm].sl[1];
        cpu_state_high.XMM[cpu_reg].f2[2] = cpu_state_high.XMM[cpu_rm].sl[2];
        cpu_state_high.XMM[cpu_reg].f2[3] = cpu_state_high.XMM[cpu_rm].sl[3];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[2]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[3]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        dst[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        dst[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].f2[0] = dst[0];
        cpu_state_high.XMM[cpu_reg].f2[1] = dst[1];
        cpu_state_high.XMM[cpu_reg].f2[2] = dst[2];
        cpu_state_high.XMM[cpu_reg].f2[3] = dst[3];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[0]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[1]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[2]);
        //check_sse_exceptions_float(&cpu_state_high.XMM[cpu_reg].f2[3]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTTPS2DQ_xmm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(cpu_state_high.XMM[cpu_rm].f2[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(cpu_state_high.XMM[cpu_rm].f2[1]);
        cpu_state_high.XMM[cpu_reg].sl[2] = trunc(cpu_state_high.XMM[cpu_rm].f2[2]);
        cpu_state_high.XMM[cpu_reg].sl[3] = trunc(cpu_state_high.XMM[cpu_rm].f2[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        dst[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        dst[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float dst_real[4];
        dst_real[0]       = *(float *) &dst[0];
        dst_real[1]       = *(float *) &dst[1];
        dst_real[2]       = *(float *) &dst[2];
        dst_real[3]       = *(float *) &dst[3];
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(dst_real[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(dst_real[1]);
        cpu_state_high.XMM[cpu_reg].sl[2] = trunc(dst_real[2]);
        cpu_state_high.XMM[cpu_reg].sl[3] = trunc(dst_real[3]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTTPS2DQ_xmm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(cpu_state_high.XMM[cpu_rm].f2[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(cpu_state_high.XMM[cpu_rm].f2[1]);
        cpu_state_high.XMM[cpu_reg].sl[2] = trunc(cpu_state_high.XMM[cpu_rm].f2[2]);
        cpu_state_high.XMM[cpu_reg].sl[3] = trunc(cpu_state_high.XMM[cpu_rm].f2[3]);
        CLOCK_CYCLES(1);
    } else {
        uint32_t dst[4];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        dst[2] = readmeml(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        dst[3] = readmeml(easeg, cpu_state.eaaddr + 12);
        if (cpu_state.abrt)
            return 1;
        float dst_real[4];
        dst_real[0]       = *(float *) &dst[0];
        dst_real[1]       = *(float *) &dst[1];
        dst_real[2]       = *(float *) &dst[2];
        dst_real[3]       = *(float *) &dst[3];
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(dst_real[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(dst_real[1]);
        cpu_state_high.XMM[cpu_reg].sl[2] = trunc(dst_real[2]);
        cpu_state_high.XMM[cpu_reg].sl[3] = trunc(dst_real[3]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPD2DQ_mm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].sl[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        cpu_state_high.XMM[cpu_reg].sl[1] = cpu_state_high.XMM[cpu_rm].d2[1];
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0] = *(double *) &dst[0];
        dst_real[1] = *(double *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].sl[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].sl[1] = dst_real[1];
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPD2DQ_mm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].sl[0] = cpu_state_high.XMM[cpu_rm].d2[0];
        cpu_state_high.XMM[cpu_reg].sl[1] = cpu_state_high.XMM[cpu_rm].d2[1];
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0] = *(double *) &dst[0];
        dst_real[1] = *(double *) &dst[1];
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].sl[0] = dst_real[0];
        cpu_state_high.XMM[cpu_reg].sl[1] = dst_real[1];
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        fesetround(FE_TONEAREST);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTTPD2DQ_mm_xmm_a16(uint32_t fetchdat)
{
    fetch_ea_16(fetchdat);
    ILLEGAL_ON(!sse_xmm);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(cpu_state_high.XMM[cpu_rm].d2[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(cpu_state_high.XMM[cpu_rm].d2[1]);
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0]       = *(double *) &dst[0];
        dst_real[1]       = *(double *) &dst[1];
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(dst_real[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(dst_real[1]);
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTTPD2DQ_mm_xmm_a32(uint32_t fetchdat)
{
    fetch_ea_32(fetchdat);
    ILLEGAL_ON(!sse_xmm);
    if (cpu_mod == 3) {
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(cpu_state_high.XMM[cpu_rm].d2[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(cpu_state_high.XMM[cpu_rm].d2[1]);
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        CLOCK_CYCLES(1);
    } else {
        uint64_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmemq(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmemq(easeg, cpu_state.eaaddr + 8);
        if (cpu_state.abrt)
            return 1;
        double dst_real[2];
        dst_real[0]       = *(double *) &dst[0];
        dst_real[1]       = *(double *) &dst[1];
        cpu_state_high.XMM[cpu_reg].sl[0] = trunc(dst_real[0]);
        cpu_state_high.XMM[cpu_reg].sl[1] = trunc(dst_real[1]);
        cpu_state_high.XMM[cpu_reg].q[1] = 0;
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTDQ2PD_mm_xmm_a16(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].sl[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = cpu_state_high.XMM[cpu_rm].sl[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = dst[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTDQ2PD_mm_xmm_a32(uint32_t fetchdat)
{
    feclearexcept(FE_ALL_EXCEPT);
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = cpu_state_high.XMM[cpu_rm].sl[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = cpu_state_high.XMM[cpu_rm].sl[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(1);
    } else {
        int32_t dst[2];

        SEG_CHECK_READ(cpu_state.ea_seg);
        dst[0] = readmeml(easeg, cpu_state.eaaddr);
        if (cpu_state.abrt)
            return 1;
        dst[1] = readmeml(easeg, cpu_state.eaaddr + 4);
        if (cpu_state.abrt)
            return 1;
        fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
        cpu_state_high.XMM[cpu_reg].d2[0] = dst[0];
        cpu_state_high.XMM[cpu_reg].d2[1] = dst[1];
        fesetround(FE_TONEAREST);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[0]);
        //check_sse_exceptions_double(&cpu_state_high.XMM[cpu_reg].d2[1]);
        CLOCK_CYCLES(2);
    }
    return 0;
}

static int
opCVTPD2PI_mm_xmm_a16(uint32_t fetchdat)
{
    SSE_REG  src;
    MMX_REG *dst;
    MMX_ENTER();
    fetch_ea_16(fetchdat);

    dst = MMX_GETREGP(cpu_reg);
    SSE_GETSRC();
    
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    dst->sl[0] = src.d2[0];
    dst->sl[1] = src.d2[1];
    fesetround(FE_TONEAREST);
    return 0;
}

static int
opCVTPD2PI_mm_xmm_a32(uint32_t fetchdat)
{
    SSE_REG  src;
    MMX_REG *dst;
    MMX_ENTER();
    fetch_ea_32     (fetchdat);

    dst = MMX_GETREGP(cpu_reg);
    SSE_GETSRC();
    
    fesetround(rounding_modes[(cpu_state_high.mxcsr >> 13) & 3]);
    dst->sl[0] = src.d2[0];
    dst->sl[1] = src.d2[1];
    fesetround(FE_TONEAREST);
    return 0;
}
