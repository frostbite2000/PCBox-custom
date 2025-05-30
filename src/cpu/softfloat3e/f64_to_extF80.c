/*============================================================================

This C source file is part of the SoftFloat IEEE Floating-Point Arithmetic
Package, Release 3e, by John R. Hauser.

Copyright 2011, 2012, 2013, 2014, 2015 The Regents of the University of
California.  All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
    this list of conditions, and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions, and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

 3. Neither the name of the University nor the names of its contributors may
    be used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS "AS IS", AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE
DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include <stdbool.h>
#include <stdint.h>
#include "internals.h"
#include "specialize.h"
#include "softfloat.h"

extFloat80_t f64_to_extF80(float64 a, struct softfloat_status_t *status)
{
    bool sign;
    int16_t exp;
    uint64_t frac;
    struct commonNaN commonNaN;
    struct uint128 uiZ;
    uint16_t uiZ64;
    uint64_t uiZ0;
    struct exp16_sig64 normExpSig;

    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    sign = signF64UI(a);
    exp  = expF64UI(a);
    frac = fracF64UI(a);
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (exp == 0x7FF) {
        if (frac) {
            softfloat_f64UIToCommonNaN(a, &commonNaN, status);
            uiZ = softfloat_commonNaNToExtF80UI(&commonNaN);
            uiZ64 = uiZ.v64;
            uiZ0  = uiZ.v0;
        } else {
            uiZ64 = packToExtF80UI64(sign, 0x7FFF);
            uiZ0  = 0x8000000000000000ULL;
        }
        return packToExtF80_twoargs(uiZ64, uiZ0);
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    if (! exp) {
        if (! frac) {
            return packToExtF80(sign, 0, 0);
        }
        softfloat_raiseFlags(status, softfloat_flag_denormal);
        normExpSig = softfloat_normSubnormalF64Sig(frac);
        exp = normExpSig.exp;
        frac = normExpSig.sig;
    }
    /*------------------------------------------------------------------------
    *------------------------------------------------------------------------*/
    uiZ64 = packToExtF80UI64(sign, exp + 0x3C00);
    uiZ0  = (frac | 0x0010000000000000ULL)<<11;
    return packToExtF80_twoargs(uiZ64, uiZ0);
}
