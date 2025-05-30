/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdlib.h>
#include <math.h>

static int
opSQRTPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSQRTPD_xmm_xmm_a16(fetchdat);
    
    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sqrt(src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_sqrt(src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_sqrt(src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_sqrt(src.f[3], &status);

    return 0;
}

static int
opSQRTPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSQRTPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sqrt(src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_sqrt(src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_sqrt(src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_sqrt(src.f[3], &status);

    return 0;
}

static int
opSQRTSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sqrt(src.f[0], &status);

    return 0;
}

static int
opSQRTSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sqrt(src.f[0], &status);

    return 0;
}

/* convert float32 NaN number to QNaN */
static float32 f32_convert_to_QNaN(float32 op)
{
  return op | 0x7FC00000;
}

static const uint16_t rsqrt_table0[1024] = {
    0x34f8, 0x34e0, 0x34d0, 0x34b8, 0x34a0, 0x3488, 0x3470, 0x3460,
    0x3448, 0x3430, 0x3418, 0x3400, 0x33f0, 0x33d8, 0x33c0, 0x33a8,
    0x3398, 0x3380, 0x3368, 0x3350, 0x3338, 0x3328, 0x3310, 0x32f8,
    0x32e8, 0x32d0, 0x32b8, 0x32a0, 0x3290, 0x3278, 0x3260, 0x3250,
    0x3238, 0x3220, 0x3208, 0x31f8, 0x31e0, 0x31c8, 0x31b8, 0x31a0,
    0x3188, 0x3178, 0x3160, 0x3148, 0x3138, 0x3120, 0x3108, 0x30f8,
    0x30e0, 0x30c8, 0x30b8, 0x30a0, 0x3090, 0x3078, 0x3060, 0x3050,
    0x3038, 0x3028, 0x3010, 0x2ff8, 0x2fe8, 0x2fd0, 0x2fc0, 0x2fa8,
    0x2f90, 0x2f80, 0x2f68, 0x2f58, 0x2f40, 0x2f30, 0x2f18, 0x2f00,
    0x2ef0, 0x2ed8, 0x2ec8, 0x2eb0, 0x2ea0, 0x2e88, 0x2e78, 0x2e60,
    0x2e50, 0x2e38, 0x2e20, 0x2e10, 0x2df8, 0x2de8, 0x2dd0, 0x2dc0,
    0x2da8, 0x2d98, 0x2d80, 0x2d70, 0x2d58, 0x2d48, 0x2d38, 0x2d20,
    0x2d10, 0x2cf8, 0x2ce8, 0x2cd0, 0x2cc0, 0x2ca8, 0x2c98, 0x2c80,
    0x2c70, 0x2c58, 0x2c48, 0x2c38, 0x2c20, 0x2c10, 0x2bf8, 0x2be8,
    0x2bd0, 0x2bc0, 0x2bb0, 0x2b98, 0x2b88, 0x2b70, 0x2b60, 0x2b50,
    0x2b38, 0x2b28, 0x2b10, 0x2b00, 0x2af0, 0x2ad8, 0x2ac8, 0x2ab8,
    0x2aa0, 0x2a90, 0x2a78, 0x2a68, 0x2a58, 0x2a40, 0x2a30, 0x2a20,
    0x2a08, 0x29f8, 0x29e8, 0x29d0, 0x29c0, 0x29b0, 0x2998, 0x2988,
    0x2978, 0x2960, 0x2950, 0x2940, 0x2928, 0x2918, 0x2908, 0x28f0,
    0x28e0, 0x28d0, 0x28c0, 0x28a8, 0x2898, 0x2888, 0x2870, 0x2860,
    0x2850, 0x2840, 0x2828, 0x2818, 0x2808, 0x27f8, 0x27e0, 0x27d0,
    0x27c0, 0x27b0, 0x2798, 0x2788, 0x2778, 0x2768, 0x2750, 0x2740,
    0x2730, 0x2720, 0x2708, 0x26f8, 0x26e8, 0x26d8, 0x26c8, 0x26b0,
    0x26a0, 0x2690, 0x2680, 0x2670, 0x2658, 0x2648, 0x2638, 0x2628,
    0x2618, 0x2600, 0x25f0, 0x25e0, 0x25d0, 0x25c0, 0x25b0, 0x2598,
    0x2588, 0x2578, 0x2568, 0x2558, 0x2548, 0x2530, 0x2520, 0x2510,
    0x2500, 0x24f0, 0x24e0, 0x24d0, 0x24b8, 0x24a8, 0x2498, 0x2488,
    0x2478, 0x2468, 0x2458, 0x2448, 0x2430, 0x2420, 0x2410, 0x2400,
    0x23f0, 0x23e0, 0x23d0, 0x23c0, 0x23b0, 0x23a0, 0x2388, 0x2378,
    0x2368, 0x2358, 0x2348, 0x2338, 0x2328, 0x2318, 0x2308, 0x22f8,
    0x22e8, 0x22d8, 0x22c8, 0x22b8, 0x22a8, 0x2290, 0x2280, 0x2270,
    0x2260, 0x2250, 0x2240, 0x2230, 0x2220, 0x2210, 0x2200, 0x21f0,
    0x21e0, 0x21d0, 0x21c0, 0x21b0, 0x21a0, 0x2190, 0x2180, 0x2170,
    0x2160, 0x2150, 0x2140, 0x2130, 0x2120, 0x2110, 0x2100, 0x20f0,
    0x20e0, 0x20d0, 0x20c0, 0x20b0, 0x20a0, 0x2090, 0x2080, 0x2070,
    0x2060, 0x2050, 0x2040, 0x2030, 0x2020, 0x2010, 0x2000, 0x1ff0,
    0x1fe8, 0x1fd8, 0x1fc8, 0x1fb8, 0x1fa8, 0x1f98, 0x1f88, 0x1f78,
    0x1f68, 0x1f58, 0x1f48, 0x1f38, 0x1f28, 0x1f18, 0x1f08, 0x1f00,
    0x1ef0, 0x1ee0, 0x1ed0, 0x1ec0, 0x1eb0, 0x1ea0, 0x1e90, 0x1e80,
    0x1e70, 0x1e60, 0x1e58, 0x1e48, 0x1e38, 0x1e28, 0x1e18, 0x1e08,
    0x1df8, 0x1de8, 0x1de0, 0x1dd0, 0x1dc0, 0x1db0, 0x1da0, 0x1d90,
    0x1d80, 0x1d70, 0x1d68, 0x1d58, 0x1d48, 0x1d38, 0x1d28, 0x1d18,
    0x1d08, 0x1d00, 0x1cf0, 0x1ce0, 0x1cd0, 0x1cc0, 0x1cb0, 0x1ca8,
    0x1c98, 0x1c88, 0x1c78, 0x1c68, 0x1c58, 0x1c50, 0x1c40, 0x1c30,
    0x1c20, 0x1c10, 0x1c08, 0x1bf8, 0x1be8, 0x1bd8, 0x1bc8, 0x1bc0,
    0x1bb0, 0x1ba0, 0x1b90, 0x1b80, 0x1b78, 0x1b68, 0x1b58, 0x1b48,
    0x1b38, 0x1b30, 0x1b20, 0x1b10, 0x1b00, 0x1af0, 0x1ae8, 0x1ad8,
    0x1ac8, 0x1ab8, 0x1ab0, 0x1aa0, 0x1a90, 0x1a80, 0x1a78, 0x1a68,
    0x1a58, 0x1a48, 0x1a40, 0x1a30, 0x1a20, 0x1a10, 0x1a08, 0x19f8,
    0x19e8, 0x19d8, 0x19d0, 0x19c0, 0x19b0, 0x19a0, 0x1998, 0x1988,
    0x1978, 0x1970, 0x1960, 0x1950, 0x1940, 0x1938, 0x1928, 0x1918,
    0x1910, 0x1900, 0x18f0, 0x18e0, 0x18d8, 0x18c8, 0x18b8, 0x18b0,
    0x18a0, 0x1890, 0x1888, 0x1878, 0x1868, 0x1858, 0x1850, 0x1840,
    0x1830, 0x1828, 0x1818, 0x1808, 0x1800, 0x17f0, 0x17e0, 0x17d8,
    0x17c8, 0x17b8, 0x17b0, 0x17a0, 0x1790, 0x1788, 0x1778, 0x1768,
    0x1760, 0x1750, 0x1740, 0x1738, 0x1728, 0x1718, 0x1710, 0x1700,
    0x16f8, 0x16e8, 0x16d8, 0x16d0, 0x16c0, 0x16b0, 0x16a8, 0x1698,
    0x1688, 0x1680, 0x1670, 0x1668, 0x1658, 0x1648, 0x1640, 0x1630,
    0x1628, 0x1618, 0x1608, 0x1600, 0x15f0, 0x15e0, 0x15d8, 0x15c8,
    0x15c0, 0x15b0, 0x15a0, 0x1598, 0x1588, 0x1580, 0x1570, 0x1560,
    0x1558, 0x1548, 0x1540, 0x1530, 0x1528, 0x1518, 0x1508, 0x1500,
    0x14f0, 0x14e8, 0x14d8, 0x14d0, 0x14c0, 0x14b0, 0x14a8, 0x1498,
    0x1490, 0x1480, 0x1478, 0x1468, 0x1458, 0x1450, 0x1440, 0x1438,
    0x1428, 0x1420, 0x1410, 0x1408, 0x13f8, 0x13f0, 0x13e0, 0x13d0,
    0x13c8, 0x13b8, 0x13b0, 0x13a0, 0x1398, 0x1388, 0x1380, 0x1370,
    0x1368, 0x1358, 0x1350, 0x1340, 0x1338, 0x1328, 0x1318, 0x1310,
    0x1300, 0x12f8, 0x12e8, 0x12e0, 0x12d0, 0x12c8, 0x12b8, 0x12b0,
    0x12a0, 0x1298, 0x1288, 0x1280, 0x1270, 0x1268, 0x1258, 0x1250,
    0x1240, 0x1238, 0x1228, 0x1220, 0x1210, 0x1208, 0x11f8, 0x11f0,
    0x11e8, 0x11d8, 0x11d0, 0x11c0, 0x11b8, 0x11a8, 0x11a0, 0x1190,
    0x1188, 0x1178, 0x1170, 0x1160, 0x1158, 0x1148, 0x1140, 0x1130,
    0x1128, 0x1120, 0x1110, 0x1108, 0x10f8, 0x10f0, 0x10e0, 0x10d8,
    0x10c8, 0x10c0, 0x10b0, 0x10a8, 0x10a0, 0x1090, 0x1088, 0x1078,
    0x1070, 0x1060, 0x1058, 0x1050, 0x1040, 0x1038, 0x1028, 0x1020,
    0x1010, 0x1008, 0x1000, 0x0ff0, 0x0fe8, 0x0fd8, 0x0fd0, 0x0fc0,
    0x0fb8, 0x0fb0, 0x0fa0, 0x0f98, 0x0f88, 0x0f80, 0x0f78, 0x0f68,
    0x0f60, 0x0f50, 0x0f48, 0x0f40, 0x0f30, 0x0f28, 0x0f18, 0x0f10,
    0x0f08, 0x0ef8, 0x0ef0, 0x0ee0, 0x0ed8, 0x0ed0, 0x0ec0, 0x0eb8,
    0x0ea8, 0x0ea0, 0x0e98, 0x0e88, 0x0e80, 0x0e78, 0x0e68, 0x0e60,
    0x0e50, 0x0e48, 0x0e40, 0x0e30, 0x0e28, 0x0e20, 0x0e10, 0x0e08,
    0x0df8, 0x0df0, 0x0de8, 0x0dd8, 0x0dd0, 0x0dc8, 0x0db8, 0x0db0,
    0x0da8, 0x0d98, 0x0d90, 0x0d80, 0x0d78, 0x0d70, 0x0d60, 0x0d58,
    0x0d50, 0x0d40, 0x0d38, 0x0d30, 0x0d20, 0x0d18, 0x0d10, 0x0d00,
    0x0cf8, 0x0cf0, 0x0ce0, 0x0cd8, 0x0cd0, 0x0cc0, 0x0cb8, 0x0cb0,
    0x0ca0, 0x0c98, 0x0c90, 0x0c80, 0x0c78, 0x0c70, 0x0c60, 0x0c58,
    0x0c50, 0x0c40, 0x0c38, 0x0c30, 0x0c28, 0x0c18, 0x0c10, 0x0c08,
    0x0bf8, 0x0bf0, 0x0be8, 0x0bd8, 0x0bd0, 0x0bc8, 0x0bb8, 0x0bb0,
    0x0ba8, 0x0ba0, 0x0b90, 0x0b88, 0x0b80, 0x0b70, 0x0b68, 0x0b60,
    0x0b58, 0x0b48, 0x0b40, 0x0b38, 0x0b28, 0x0b20, 0x0b18, 0x0b10,
    0x0b00, 0x0af8, 0x0af0, 0x0ae0, 0x0ad8, 0x0ad0, 0x0ac8, 0x0ab8,
    0x0ab0, 0x0aa8, 0x0a98, 0x0a90, 0x0a88, 0x0a80, 0x0a70, 0x0a68,
    0x0a60, 0x0a58, 0x0a48, 0x0a40, 0x0a38, 0x0a30, 0x0a20, 0x0a18,
    0x0a10, 0x0a08, 0x09f8, 0x09f0, 0x09e8, 0x09e0, 0x09d0, 0x09c8,
    0x09c0, 0x09b8, 0x09a8, 0x09a0, 0x0998, 0x0990, 0x0980, 0x0978,
    0x0970, 0x0968, 0x0958, 0x0950, 0x0948, 0x0940, 0x0930, 0x0928,
    0x0920, 0x0918, 0x0910, 0x0900, 0x08f8, 0x08f0, 0x08e8, 0x08d8,
    0x08d0, 0x08c8, 0x08c0, 0x08b8, 0x08a8, 0x08a0, 0x0898, 0x0890,
    0x0880, 0x0878, 0x0870, 0x0868, 0x0860, 0x0850, 0x0848, 0x0840,
    0x0838, 0x0830, 0x0820, 0x0818, 0x0810, 0x0808, 0x0800, 0x07f0,
    0x07e8, 0x07e0, 0x07d8, 0x07d0, 0x07c0, 0x07b8, 0x07b0, 0x07a8,
    0x07a0, 0x0790, 0x0788, 0x0780, 0x0778, 0x0770, 0x0768, 0x0758,
    0x0750, 0x0748, 0x0740, 0x0738, 0x0728, 0x0720, 0x0718, 0x0710,
    0x0708, 0x0700, 0x06f0, 0x06e8, 0x06e0, 0x06d8, 0x06d0, 0x06c8,
    0x06b8, 0x06b0, 0x06a8, 0x06a0, 0x0698, 0x0690, 0x0680, 0x0678,
    0x0670, 0x0668, 0x0660, 0x0658, 0x0648, 0x0640, 0x0638, 0x0630,
    0x0628, 0x0620, 0x0618, 0x0608, 0x0600, 0x05f8, 0x05f0, 0x05e8,
    0x05e0, 0x05d8, 0x05c8, 0x05c0, 0x05b8, 0x05b0, 0x05a8, 0x05a0,
    0x0598, 0x0588, 0x0580, 0x0578, 0x0570, 0x0568, 0x0560, 0x0558,
    0x0548, 0x0540, 0x0538, 0x0530, 0x0528, 0x0520, 0x0518, 0x0510,
    0x0500, 0x04f8, 0x04f0, 0x04e8, 0x04e0, 0x04d8, 0x04d0, 0x04c8,
    0x04c0, 0x04b0, 0x04a8, 0x04a0, 0x0498, 0x0490, 0x0488, 0x0480,
    0x0478, 0x0470, 0x0460, 0x0458, 0x0450, 0x0448, 0x0440, 0x0438,
    0x0430, 0x0428, 0x0420, 0x0410, 0x0408, 0x0400, 0x03f8, 0x03f0,
    0x03e8, 0x03e0, 0x03d8, 0x03d0, 0x03c8, 0x03c0, 0x03b0, 0x03a8,
    0x03a0, 0x0398, 0x0390, 0x0388, 0x0380, 0x0378, 0x0370, 0x0368,
    0x0360, 0x0358, 0x0348, 0x0340, 0x0338, 0x0330, 0x0328, 0x0320,
    0x0318, 0x0310, 0x0308, 0x0300, 0x02f8, 0x02f0, 0x02e8, 0x02d8,
    0x02d0, 0x02c8, 0x02c0, 0x02b8, 0x02b0, 0x02a8, 0x02a0, 0x0298,
    0x0290, 0x0288, 0x0280, 0x0278, 0x0270, 0x0268, 0x0260, 0x0250,
    0x0248, 0x0240, 0x0238, 0x0230, 0x0228, 0x0220, 0x0218, 0x0210,
    0x0208, 0x0200, 0x01f8, 0x01f0, 0x01e8, 0x01e0, 0x01d8, 0x01d0,
    0x01c8, 0x01c0, 0x01b8, 0x01b0, 0x01a0, 0x0198, 0x0190, 0x0188,
    0x0180, 0x0178, 0x0170, 0x0168, 0x0160, 0x0158, 0x0150, 0x0148,
    0x0140, 0x0138, 0x0130, 0x0128, 0x0120, 0x0118, 0x0110, 0x0108,
    0x0100, 0x00f8, 0x00f0, 0x00e8, 0x00e0, 0x00d8, 0x00d0, 0x00c8,
    0x00c0, 0x00b8, 0x00b0, 0x00a8, 0x00a0, 0x0098, 0x0090, 0x0088,
    0x0080, 0x0078, 0x0070, 0x0068, 0x0060, 0x0058, 0x0050, 0x0048,
    0x0040, 0x0038, 0x0030, 0x0028, 0x0020, 0x0018, 0x0010, 0x0008
};

static const uint16_t rsqrt_table1[1024] = {
    0x7ff0, 0x7fd0, 0x7fb0, 0x7f90, 0x7f70, 0x7f50, 0x7f30, 0x7f10,
    0x7ef0, 0x7ed0, 0x7eb0, 0x7e90, 0x7e70, 0x7e58, 0x7e38, 0x7e18,
    0x7df8, 0x7dd8, 0x7db8, 0x7d98, 0x7d78, 0x7d58, 0x7d38, 0x7d20,
    0x7d00, 0x7ce0, 0x7cc0, 0x7ca0, 0x7c80, 0x7c60, 0x7c48, 0x7c28,
    0x7c08, 0x7be8, 0x7bc8, 0x7bb0, 0x7b90, 0x7b70, 0x7b50, 0x7b30,
    0x7b18, 0x7af8, 0x7ad8, 0x7ab8, 0x7aa0, 0x7a80, 0x7a60, 0x7a40,
    0x7a28, 0x7a08, 0x79e8, 0x79c8, 0x79b0, 0x7990, 0x7970, 0x7958,
    0x7938, 0x7918, 0x7900, 0x78e0, 0x78c0, 0x78a8, 0x7888, 0x7868,
    0x7850, 0x7830, 0x7810, 0x77f8, 0x77d8, 0x77b8, 0x77a0, 0x7780,
    0x7768, 0x7748, 0x7728, 0x7710, 0x76f0, 0x76d8, 0x76b8, 0x7698,
    0x7680, 0x7660, 0x7648, 0x7628, 0x7610, 0x75f0, 0x75d0, 0x75b8,
    0x7598, 0x7580, 0x7560, 0x7548, 0x7528, 0x7510, 0x74f0, 0x74d8,
    0x74b8, 0x74a0, 0x7480, 0x7468, 0x7448, 0x7430, 0x7410, 0x73f8,
    0x73d8, 0x73c0, 0x73a8, 0x7388, 0x7370, 0x7350, 0x7338, 0x7318,
    0x7300, 0x72e8, 0x72c8, 0x72b0, 0x7290, 0x7278, 0x7260, 0x7240,
    0x7228, 0x7208, 0x71f0, 0x71d8, 0x71b8, 0x71a0, 0x7188, 0x7168,
    0x7150, 0x7130, 0x7118, 0x7100, 0x70e0, 0x70c8, 0x70b0, 0x7090,
    0x7078, 0x7060, 0x7048, 0x7028, 0x7010, 0x6ff8, 0x6fd8, 0x6fc0,
    0x6fa8, 0x6f88, 0x6f70, 0x6f58, 0x6f40, 0x6f20, 0x6f08, 0x6ef0,
    0x6ed8, 0x6eb8, 0x6ea0, 0x6e88, 0x6e70, 0x6e50, 0x6e38, 0x6e20,
    0x6e08, 0x6df0, 0x6dd0, 0x6db8, 0x6da0, 0x6d88, 0x6d70, 0x6d50,
    0x6d38, 0x6d20, 0x6d08, 0x6cf0, 0x6cd8, 0x6cb8, 0x6ca0, 0x6c88,
    0x6c70, 0x6c58, 0x6c40, 0x6c20, 0x6c08, 0x6bf0, 0x6bd8, 0x6bc0,
    0x6ba8, 0x6b90, 0x6b78, 0x6b58, 0x6b40, 0x6b28, 0x6b10, 0x6af8,
    0x6ae0, 0x6ac8, 0x6ab0, 0x6a98, 0x6a80, 0x6a68, 0x6a48, 0x6a30,
    0x6a18, 0x6a00, 0x69e8, 0x69d0, 0x69b8, 0x69a0, 0x6988, 0x6970,
    0x6958, 0x6940, 0x6928, 0x6910, 0x68f8, 0x68e0, 0x68c8, 0x68b0,
    0x6898, 0x6880, 0x6868, 0x6850, 0x6838, 0x6820, 0x6808, 0x67f0,
    0x67d8, 0x67c0, 0x67a8, 0x6790, 0x6778, 0x6760, 0x6748, 0x6730,
    0x6718, 0x6700, 0x66e8, 0x66d8, 0x66c0, 0x66a8, 0x6690, 0x6678,
    0x6660, 0x6648, 0x6630, 0x6618, 0x6600, 0x65e8, 0x65d0, 0x65c0,
    0x65a8, 0x6590, 0x6578, 0x6560, 0x6548, 0x6530, 0x6518, 0x6508,
    0x64f0, 0x64d8, 0x64c0, 0x64a8, 0x6490, 0x6478, 0x6468, 0x6450,
    0x6438, 0x6420, 0x6408, 0x63f0, 0x63e0, 0x63c8, 0x63b0, 0x6398,
    0x6380, 0x6370, 0x6358, 0x6340, 0x6328, 0x6310, 0x6300, 0x62e8,
    0x62d0, 0x62b8, 0x62a0, 0x6290, 0x6278, 0x6260, 0x6248, 0x6238,
    0x6220, 0x6208, 0x61f0, 0x61e0, 0x61c8, 0x61b0, 0x6198, 0x6188,
    0x6170, 0x6158, 0x6140, 0x6130, 0x6118, 0x6100, 0x60f0, 0x60d8,
    0x60c0, 0x60a8, 0x6098, 0x6080, 0x6068, 0x6058, 0x6040, 0x6028,
    0x6018, 0x6000, 0x5fe8, 0x5fd8, 0x5fc0, 0x5fa8, 0x5f98, 0x5f80,
    0x5f68, 0x5f58, 0x5f40, 0x5f28, 0x5f18, 0x5f00, 0x5ee8, 0x5ed8,
    0x5ec0, 0x5ea8, 0x5e98, 0x5e80, 0x5e70, 0x5e58, 0x5e40, 0x5e30,
    0x5e18, 0x5e00, 0x5df0, 0x5dd8, 0x5dc8, 0x5db0, 0x5d98, 0x5d88,
    0x5d70, 0x5d60, 0x5d48, 0x5d38, 0x5d20, 0x5d08, 0x5cf8, 0x5ce0,
    0x5cd0, 0x5cb8, 0x5ca8, 0x5c90, 0x5c78, 0x5c68, 0x5c50, 0x5c40,
    0x5c28, 0x5c18, 0x5c00, 0x5bf0, 0x5bd8, 0x5bc8, 0x5bb0, 0x5b98,
    0x5b88, 0x5b70, 0x5b60, 0x5b48, 0x5b38, 0x5b20, 0x5b10, 0x5af8,
    0x5ae8, 0x5ad0, 0x5ac0, 0x5aa8, 0x5a98, 0x5a80, 0x5a70, 0x5a58,
    0x5a48, 0x5a30, 0x5a20, 0x5a08, 0x59f8, 0x59e8, 0x59d0, 0x59c0,
    0x59a8, 0x5998, 0x5980, 0x5970, 0x5958, 0x5948, 0x5930, 0x5920,
    0x5910, 0x58f8, 0x58e8, 0x58d0, 0x58c0, 0x58a8, 0x5898, 0x5888,
    0x5870, 0x5860, 0x5848, 0x5838, 0x5828, 0x5810, 0x5800, 0x57e8,
    0x57d8, 0x57c8, 0x57b0, 0x57a0, 0x5788, 0x5778, 0x5768, 0x5750,
    0x5740, 0x5728, 0x5718, 0x5708, 0x56f0, 0x56e0, 0x56d0, 0x56b8,
    0x56a8, 0x5698, 0x5680, 0x5670, 0x5658, 0x5648, 0x5638, 0x5620,
    0x5610, 0x5600, 0x55e8, 0x55d8, 0x55c8, 0x55b0, 0x55a0, 0x5590,
    0x5578, 0x5568, 0x5558, 0x5540, 0x5530, 0x5520, 0x5510, 0x54f8,
    0x54e8, 0x54d8, 0x54c0, 0x54b0, 0x54a0, 0x5488, 0x5478, 0x5468,
    0x5458, 0x5440, 0x5430, 0x5420, 0x5410, 0x53f8, 0x53e8, 0x53d8,
    0x53c0, 0x53b0, 0x53a0, 0x5390, 0x5378, 0x5368, 0x5358, 0x5348,
    0x5330, 0x5320, 0x5310, 0x5300, 0x52e8, 0x52d8, 0x52c8, 0x52b8,
    0x52a8, 0x5290, 0x5280, 0x5270, 0x5260, 0x5248, 0x5238, 0x5228,
    0x5218, 0x5208, 0x51f0, 0x51e0, 0x51d0, 0x51c0, 0x51b0, 0x5198,
    0x5188, 0x5178, 0x5168, 0x5158, 0x5140, 0x5130, 0x5120, 0x5110,
    0x5100, 0x50e8, 0x50d8, 0x50c8, 0x50b8, 0x50a8, 0x5098, 0x5080,
    0x5070, 0x5060, 0x5050, 0x5040, 0x5030, 0x5020, 0x5008, 0x4ff8,
    0x4fe8, 0x4fd8, 0x4fc8, 0x4fb8, 0x4fa8, 0x4f90, 0x4f80, 0x4f70,
    0x4f60, 0x4f50, 0x4f40, 0x4f30, 0x4f20, 0x4f08, 0x4ef8, 0x4ee8,
    0x4ed8, 0x4ec8, 0x4eb8, 0x4ea8, 0x4e98, 0x4e88, 0x4e70, 0x4e60,
    0x4e50, 0x4e40, 0x4e30, 0x4e20, 0x4e10, 0x4e00, 0x4df0, 0x4de0,
    0x4dd0, 0x4db8, 0x4da8, 0x4d98, 0x4d88, 0x4d78, 0x4d68, 0x4d58,
    0x4d48, 0x4d38, 0x4d28, 0x4d18, 0x4d08, 0x4cf8, 0x4ce8, 0x4cd8,
    0x4cc8, 0x4cb8, 0x4ca0, 0x4c90, 0x4c80, 0x4c70, 0x4c60, 0x4c50,
    0x4c40, 0x4c30, 0x4c20, 0x4c10, 0x4c00, 0x4bf0, 0x4be0, 0x4bd0,
    0x4bc0, 0x4bb0, 0x4ba0, 0x4b90, 0x4b80, 0x4b70, 0x4b60, 0x4b50,
    0x4b40, 0x4b30, 0x4b20, 0x4b10, 0x4b00, 0x4af0, 0x4ae0, 0x4ad0,
    0x4ac0, 0x4ab0, 0x4aa0, 0x4a90, 0x4a80, 0x4a70, 0x4a60, 0x4a50,
    0x4a40, 0x4a30, 0x4a20, 0x4a10, 0x4a00, 0x49f0, 0x49e0, 0x49d0,
    0x49c0, 0x49b8, 0x49a8, 0x4998, 0x4988, 0x4978, 0x4968, 0x4958,
    0x4948, 0x4938, 0x4928, 0x4918, 0x4908, 0x48f8, 0x48e8, 0x48d8,
    0x48c8, 0x48b8, 0x48b0, 0x48a0, 0x4890, 0x4880, 0x4870, 0x4860,
    0x4850, 0x4840, 0x4830, 0x4820, 0x4810, 0x4800, 0x47f8, 0x47e8,
    0x47d8, 0x47c8, 0x47b8, 0x47a8, 0x4798, 0x4788, 0x4778, 0x4768,
    0x4760, 0x4750, 0x4740, 0x4730, 0x4720, 0x4710, 0x4700, 0x46f0,
    0x46e0, 0x46d8, 0x46c8, 0x46b8, 0x46a8, 0x4698, 0x4688, 0x4678,
    0x4670, 0x4660, 0x4650, 0x4640, 0x4630, 0x4620, 0x4610, 0x4608,
    0x45f8, 0x45e8, 0x45d8, 0x45c8, 0x45b8, 0x45a8, 0x45a0, 0x4590,
    0x4580, 0x4570, 0x4560, 0x4550, 0x4548, 0x4538, 0x4528, 0x4518,
    0x4508, 0x44f8, 0x44f0, 0x44e0, 0x44d0, 0x44c0, 0x44b0, 0x44a8,
    0x4498, 0x4488, 0x4478, 0x4468, 0x4460, 0x4450, 0x4440, 0x4430,
    0x4420, 0x4418, 0x4408, 0x43f8, 0x43e8, 0x43d8, 0x43d0, 0x43c0,
    0x43b0, 0x43a0, 0x4390, 0x4388, 0x4378, 0x4368, 0x4358, 0x4350,
    0x4340, 0x4330, 0x4320, 0x4310, 0x4308, 0x42f8, 0x42e8, 0x42d8,
    0x42d0, 0x42c0, 0x42b0, 0x42a0, 0x4298, 0x4288, 0x4278, 0x4268,
    0x4260, 0x4250, 0x4240, 0x4230, 0x4228, 0x4218, 0x4208, 0x41f8,
    0x41f0, 0x41e0, 0x41d0, 0x41c0, 0x41b8, 0x41a8, 0x4198, 0x4188,
    0x4180, 0x4170, 0x4160, 0x4158, 0x4148, 0x4138, 0x4128, 0x4120,
    0x4110, 0x4100, 0x40f8, 0x40e8, 0x40d8, 0x40c8, 0x40c0, 0x40b0,
    0x40a0, 0x4098, 0x4088, 0x4078, 0x4068, 0x4060, 0x4050, 0x4040,
    0x4038, 0x4028, 0x4018, 0x4010, 0x4000, 0x3ff0, 0x3fe8, 0x3fd8,
    0x3fc8, 0x3fb8, 0x3fb0, 0x3fa0, 0x3f90, 0x3f88, 0x3f78, 0x3f68,
    0x3f60, 0x3f50, 0x3f40, 0x3f38, 0x3f28, 0x3f18, 0x3f10, 0x3f00,
    0x3ef0, 0x3ee8, 0x3ed8, 0x3ec8, 0x3ec0, 0x3eb0, 0x3ea0, 0x3e98,
    0x3e88, 0x3e80, 0x3e70, 0x3e60, 0x3e58, 0x3e48, 0x3e38, 0x3e30,
    0x3e20, 0x3e10, 0x3e08, 0x3df8, 0x3df0, 0x3de0, 0x3dd0, 0x3dc8,
    0x3db8, 0x3da8, 0x3da0, 0x3d90, 0x3d80, 0x3d78, 0x3d68, 0x3d60,
    0x3d50, 0x3d40, 0x3d38, 0x3d28, 0x3d20, 0x3d10, 0x3d00, 0x3cf8,
    0x3ce8, 0x3cd8, 0x3cd0, 0x3cc0, 0x3cb8, 0x3ca8, 0x3c98, 0x3c90,
    0x3c80, 0x3c78, 0x3c68, 0x3c58, 0x3c50, 0x3c40, 0x3c38, 0x3c28,
    0x3c20, 0x3c10, 0x3c00, 0x3bf8, 0x3be8, 0x3be0, 0x3bd0, 0x3bc0,
    0x3bb8, 0x3ba8, 0x3ba0, 0x3b90, 0x3b88, 0x3b78, 0x3b68, 0x3b60,
    0x3b50, 0x3b48, 0x3b38, 0x3b30, 0x3b20, 0x3b10, 0x3b08, 0x3af8,
    0x3af0, 0x3ae0, 0x3ad8, 0x3ac8, 0x3ac0, 0x3ab0, 0x3aa0, 0x3a98,
    0x3a88, 0x3a80, 0x3a70, 0x3a68, 0x3a58, 0x3a50, 0x3a40, 0x3a38,
    0x3a28, 0x3a20, 0x3a10, 0x3a00, 0x39f8, 0x39e8, 0x39e0, 0x39d0,
    0x39c8, 0x39b8, 0x39b0, 0x39a0, 0x3998, 0x3988, 0x3980, 0x3970,
    0x3968, 0x3958, 0x3950, 0x3940, 0x3938, 0x3928, 0x3918, 0x3910,
    0x3900, 0x38f8, 0x38e8, 0x38e0, 0x38d0, 0x38c8, 0x38b8, 0x38b0,
    0x38a0, 0x3898, 0x3888, 0x3880, 0x3870, 0x3868, 0x3858, 0x3850,
    0x3840, 0x3838, 0x3828, 0x3820, 0x3818, 0x3808, 0x3800, 0x37f0,
    0x37e8, 0x37d8, 0x37d0, 0x37c0, 0x37b8, 0x37a8, 0x37a0, 0x3790,
    0x3788, 0x3778, 0x3770, 0x3760, 0x3758, 0x3748, 0x3740, 0x3730,
    0x3728, 0x3720, 0x3710, 0x3708, 0x36f8, 0x36f0, 0x36e0, 0x36d8,
    0x36c8, 0x36c0, 0x36b0, 0x36a8, 0x3698, 0x3690, 0x3688, 0x3678,
    0x3670, 0x3660, 0x3658, 0x3648, 0x3640, 0x3630, 0x3628, 0x3620,
    0x3610, 0x3608, 0x35f8, 0x35f0, 0x35e0, 0x35d8, 0x35d0, 0x35c0,
    0x35b8, 0x35a8, 0x35a0, 0x3590, 0x3588, 0x3580, 0x3570, 0x3568,
    0x3558, 0x3550, 0x3540, 0x3538, 0x3530, 0x3520, 0x3518, 0x3508
};

// approximate reciprocal sqrt of scalar single precision FP
static float32 approximate_rsqrt(float32 op)
{
  softfloat_class_t op_class = f32_class(op);
  bool sign = f32_sign(op);

  switch(op_class) {
    case softfloat_zero:
    case softfloat_denormal:
      return packFloat32(sign, 0xFF, 0);

    case softfloat_positive_inf:
      return 0;

    case softfloat_negative_inf:
      return float32_default_nan;

    case softfloat_SNaN:
    case softfloat_QNaN:
      return f32_convert_to_QNaN(op);

    case softfloat_normalized:
      break;
  };

  if (sign == 1)
    return float32_default_nan;

  uint32_t fraction = f32_fraction(op);
  int16_t exp = f32_exp(op);

  /*
   * Calculate (1/1.yyyyyyyyyy1), the result is always rounded to the
   * 11th bit after the decimal point by round-to-nearest, regardless
   * of the current rounding mode.
   *
   * Using two precalculated 1024-entry tables.
   */

  const uint16_t *rsqrt_table = (exp & 1) ? rsqrt_table1 : rsqrt_table0;

  exp = 0x7E - ((exp - 0x7F) >> 1);

  return packFloat32(0, exp, (uint32_t)(rsqrt_table[fraction >> 13]) << 8);
}


static int
opRSQRTPS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rsqrt(src.f[0]);
    cpu_state.XMM[cpu_reg].f[1] = approximate_rsqrt(src.f[1]);
    cpu_state.XMM[cpu_reg].f[2] = approximate_rsqrt(src.f[2]);
    cpu_state.XMM[cpu_reg].f[3] = approximate_rsqrt(src.f[3]);

    return 0;
}

static int
opRSQRTPS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rsqrt(src.f[0]);
    cpu_state.XMM[cpu_reg].f[1] = approximate_rsqrt(src.f[1]);
    cpu_state.XMM[cpu_reg].f[2] = approximate_rsqrt(src.f[2]);
    cpu_state.XMM[cpu_reg].f[3] = approximate_rsqrt(src.f[3]);

    return 0;
}

static int
opRSQRTSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rsqrt(src.f[0]);

    return 0;
}

static int
opRSQRTSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rsqrt(src.f[0]);

    return 0;
}

static uint16_t rcp_table[2048] = {
    0x7ff0, 0x7fd0, 0x7fb0, 0x7f90, 0x7f70, 0x7f50, 0x7f30, 0x7f10,
    0x7ef0, 0x7ed0, 0x7eb0, 0x7e90, 0x7e70, 0x7e50, 0x7e30, 0x7e10,
    0x7df8, 0x7dd8, 0x7db8, 0x7d98, 0x7d78, 0x7d58, 0x7d38, 0x7d18,
    0x7cf8, 0x7cd8, 0x7cb8, 0x7c98, 0x7c80, 0x7c60, 0x7c40, 0x7c20,
    0x7c00, 0x7be0, 0x7bc0, 0x7ba0, 0x7b88, 0x7b68, 0x7b48, 0x7b28,
    0x7b08, 0x7ae8, 0x7ac8, 0x7ab0, 0x7a90, 0x7a70, 0x7a50, 0x7a30,
    0x7a10, 0x79f8, 0x79d8, 0x79b8, 0x7998, 0x7978, 0x7960, 0x7940,
    0x7920, 0x7900, 0x78e0, 0x78c8, 0x78a8, 0x7888, 0x7868, 0x7850,
    0x7830, 0x7810, 0x77f0, 0x77d8, 0x77b8, 0x7798, 0x7778, 0x7760,
    0x7740, 0x7720, 0x7700, 0x76e8, 0x76c8, 0x76a8, 0x7690, 0x7670,
    0x7650, 0x7630, 0x7618, 0x75f8, 0x75d8, 0x75c0, 0x75a0, 0x7580,
    0x7568, 0x7548, 0x7528, 0x7510, 0x74f0, 0x74d0, 0x74b8, 0x7498,
    0x7478, 0x7460, 0x7440, 0x7420, 0x7408, 0x73e8, 0x73d0, 0x73b0,
    0x7390, 0x7378, 0x7358, 0x7338, 0x7320, 0x7300, 0x72e8, 0x72c8,
    0x72a8, 0x7290, 0x7270, 0x7258, 0x7238, 0x7220, 0x7200, 0x71e0,
    0x71c8, 0x71a8, 0x7190, 0x7170, 0x7158, 0x7138, 0x7118, 0x7100,
    0x70e0, 0x70c8, 0x70a8, 0x7090, 0x7070, 0x7058, 0x7038, 0x7020,
    0x7000, 0x6fe8, 0x6fc8, 0x6fb0, 0x6f90, 0x6f78, 0x6f58, 0x6f40,
    0x6f20, 0x6f08, 0x6ee8, 0x6ed0, 0x6eb0, 0x6e98, 0x6e78, 0x6e60,
    0x6e40, 0x6e28, 0x6e08, 0x6df0, 0x6dd0, 0x6db8, 0x6da0, 0x6d80,
    0x6d68, 0x6d48, 0x6d30, 0x6d10, 0x6cf8, 0x6cd8, 0x6cc0, 0x6ca8,
    0x6c88, 0x6c70, 0x6c50, 0x6c38, 0x6c20, 0x6c00, 0x6be8, 0x6bc8,
    0x6bb0, 0x6b98, 0x6b78, 0x6b60, 0x6b40, 0x6b28, 0x6b10, 0x6af0,
    0x6ad8, 0x6ac0, 0x6aa0, 0x6a88, 0x6a70, 0x6a50, 0x6a38, 0x6a20,
    0x6a00, 0x69e8, 0x69c8, 0x69b0, 0x6998, 0x6978, 0x6960, 0x6948,
    0x6930, 0x6910, 0x68f8, 0x68e0, 0x68c0, 0x68a8, 0x6890, 0x6870,
    0x6858, 0x6840, 0x6820, 0x6808, 0x67f0, 0x67d8, 0x67b8, 0x67a0,
    0x6788, 0x6770, 0x6750, 0x6738, 0x6720, 0x6700, 0x66e8, 0x66d0,
    0x66b8, 0x6698, 0x6680, 0x6668, 0x6650, 0x6638, 0x6618, 0x6600,
    0x65e8, 0x65d0, 0x65b0, 0x6598, 0x6580, 0x6568, 0x6550, 0x6530,
    0x6518, 0x6500, 0x64e8, 0x64c8, 0x64b0, 0x6498, 0x6480, 0x6468,
    0x6450, 0x6430, 0x6418, 0x6400, 0x63e8, 0x63d0, 0x63b8, 0x6398,
    0x6380, 0x6368, 0x6350, 0x6338, 0x6320, 0x6300, 0x62e8, 0x62d0,
    0x62b8, 0x62a0, 0x6288, 0x6270, 0x6250, 0x6238, 0x6220, 0x6208,
    0x61f0, 0x61d8, 0x61c0, 0x61a8, 0x6190, 0x6170, 0x6158, 0x6140,
    0x6128, 0x6110, 0x60f8, 0x60e0, 0x60c8, 0x60b0, 0x6098, 0x6080,
    0x6060, 0x6048, 0x6030, 0x6018, 0x6000, 0x5fe8, 0x5fd0, 0x5fb8,
    0x5fa0, 0x5f88, 0x5f70, 0x5f58, 0x5f40, 0x5f28, 0x5f10, 0x5ef8,
    0x5ee0, 0x5ec8, 0x5eb0, 0x5e98, 0x5e80, 0x5e68, 0x5e50, 0x5e30,
    0x5e18, 0x5e00, 0x5de8, 0x5dd0, 0x5db8, 0x5da0, 0x5d88, 0x5d70,
    0x5d58, 0x5d40, 0x5d30, 0x5d18, 0x5d00, 0x5ce8, 0x5cd0, 0x5cb8,
    0x5ca0, 0x5c88, 0x5c70, 0x5c58, 0x5c40, 0x5c28, 0x5c10, 0x5bf8,
    0x5be0, 0x5bc8, 0x5bb0, 0x5b98, 0x5b80, 0x5b68, 0x5b50, 0x5b38,
    0x5b20, 0x5b08, 0x5af8, 0x5ae0, 0x5ac8, 0x5ab0, 0x5a98, 0x5a80,
    0x5a68, 0x5a50, 0x5a38, 0x5a20, 0x5a08, 0x59f8, 0x59e0, 0x59c8,
    0x59b0, 0x5998, 0x5980, 0x5968, 0x5950, 0x5938, 0x5928, 0x5910,
    0x58f8, 0x58e0, 0x58c8, 0x58b0, 0x5898, 0x5880, 0x5870, 0x5858,
    0x5840, 0x5828, 0x5810, 0x57f8, 0x57e0, 0x57d0, 0x57b8, 0x57a0,
    0x5788, 0x5770, 0x5758, 0x5748, 0x5730, 0x5718, 0x5700, 0x56e8,
    0x56d0, 0x56c0, 0x56a8, 0x5690, 0x5678, 0x5660, 0x5650, 0x5638,
    0x5620, 0x5608, 0x55f0, 0x55e0, 0x55c8, 0x55b0, 0x5598, 0x5588,
    0x5570, 0x5558, 0x5540, 0x5528, 0x5518, 0x5500, 0x54e8, 0x54d0,
    0x54c0, 0x54a8, 0x5490, 0x5478, 0x5468, 0x5450, 0x5438, 0x5420,
    0x5410, 0x53f8, 0x53e0, 0x53c8, 0x53b8, 0x53a0, 0x5388, 0x5370,
    0x5360, 0x5348, 0x5330, 0x5318, 0x5308, 0x52f0, 0x52d8, 0x52c8,
    0x52b0, 0x5298, 0x5280, 0x5270, 0x5258, 0x5240, 0x5230, 0x5218,
    0x5200, 0x51f0, 0x51d8, 0x51c0, 0x51b0, 0x5198, 0x5180, 0x5170,
    0x5158, 0x5140, 0x5128, 0x5118, 0x5100, 0x50e8, 0x50d8, 0x50c0,
    0x50a8, 0x5098, 0x5080, 0x5070, 0x5058, 0x5040, 0x5030, 0x5018,
    0x5000, 0x4ff0, 0x4fd8, 0x4fc0, 0x4fb0, 0x4f98, 0x4f80, 0x4f70,
    0x4f58, 0x4f48, 0x4f30, 0x4f18, 0x4f08, 0x4ef0, 0x4ee0, 0x4ec8,
    0x4eb0, 0x4ea0, 0x4e88, 0x4e78, 0x4e60, 0x4e48, 0x4e38, 0x4e20,
    0x4e10, 0x4df8, 0x4de0, 0x4dd0, 0x4db8, 0x4da8, 0x4d90, 0x4d78,
    0x4d68, 0x4d50, 0x4d40, 0x4d28, 0x4d18, 0x4d00, 0x4ce8, 0x4cd8,
    0x4cc0, 0x4cb0, 0x4c98, 0x4c88, 0x4c70, 0x4c60, 0x4c48, 0x4c30,
    0x4c20, 0x4c08, 0x4bf8, 0x4be0, 0x4bd0, 0x4bb8, 0x4ba8, 0x4b90,
    0x4b80, 0x4b68, 0x4b58, 0x4b40, 0x4b30, 0x4b18, 0x4b08, 0x4af0,
    0x4ad8, 0x4ac8, 0x4ab0, 0x4aa0, 0x4a88, 0x4a78, 0x4a60, 0x4a50,
    0x4a38, 0x4a28, 0x4a10, 0x4a00, 0x49e8, 0x49d8, 0x49c8, 0x49b0,
    0x49a0, 0x4988, 0x4978, 0x4960, 0x4950, 0x4938, 0x4928, 0x4910,
    0x4900, 0x48e8, 0x48d8, 0x48c0, 0x48b0, 0x4898, 0x4888, 0x4878,
    0x4860, 0x4850, 0x4838, 0x4828, 0x4810, 0x4800, 0x47e8, 0x47d8,
    0x47c8, 0x47b0, 0x47a0, 0x4788, 0x4778, 0x4760, 0x4750, 0x4740,
    0x4728, 0x4718, 0x4700, 0x46f0, 0x46d8, 0x46c8, 0x46b8, 0x46a0,
    0x4690, 0x4678, 0x4668, 0x4658, 0x4640, 0x4630, 0x4618, 0x4608,
    0x45f8, 0x45e0, 0x45d0, 0x45b8, 0x45a8, 0x4598, 0x4580, 0x4570,
    0x4560, 0x4548, 0x4538, 0x4520, 0x4510, 0x4500, 0x44e8, 0x44d8,
    0x44c8, 0x44b0, 0x44a0, 0x4488, 0x4478, 0x4468, 0x4450, 0x4440,
    0x4430, 0x4418, 0x4408, 0x43f8, 0x43e0, 0x43d0, 0x43c0, 0x43a8,
    0x4398, 0x4388, 0x4370, 0x4360, 0x4350, 0x4338, 0x4328, 0x4318,
    0x4300, 0x42f0, 0x42e0, 0x42c8, 0x42b8, 0x42a8, 0x4290, 0x4280,
    0x4270, 0x4260, 0x4248, 0x4238, 0x4228, 0x4210, 0x4200, 0x41f0,
    0x41d8, 0x41c8, 0x41b8, 0x41a8, 0x4190, 0x4180, 0x4170, 0x4158,
    0x4148, 0x4138, 0x4128, 0x4110, 0x4100, 0x40f0, 0x40d8, 0x40c8,
    0x40b8, 0x40a8, 0x4090, 0x4080, 0x4070, 0x4060, 0x4048, 0x4038,
    0x4028, 0x4018, 0x4000, 0x3ff0, 0x3fe0, 0x3fd0, 0x3fb8, 0x3fa8,
    0x3f98, 0x3f88, 0x3f70, 0x3f60, 0x3f50, 0x3f40, 0x3f28, 0x3f18,
    0x3f08, 0x3ef8, 0x3ee8, 0x3ed0, 0x3ec0, 0x3eb0, 0x3ea0, 0x3e88,
    0x3e78, 0x3e68, 0x3e58, 0x3e48, 0x3e30, 0x3e20, 0x3e10, 0x3e00,
    0x3df0, 0x3dd8, 0x3dc8, 0x3db8, 0x3da8, 0x3d98, 0x3d80, 0x3d70,
    0x3d60, 0x3d50, 0x3d40, 0x3d28, 0x3d18, 0x3d08, 0x3cf8, 0x3ce8,
    0x3cd8, 0x3cc0, 0x3cb0, 0x3ca0, 0x3c90, 0x3c80, 0x3c70, 0x3c58,
    0x3c48, 0x3c38, 0x3c28, 0x3c18, 0x3c08, 0x3bf0, 0x3be0, 0x3bd0,
    0x3bc0, 0x3bb0, 0x3ba0, 0x3b90, 0x3b78, 0x3b68, 0x3b58, 0x3b48,
    0x3b38, 0x3b28, 0x3b18, 0x3b00, 0x3af0, 0x3ae0, 0x3ad0, 0x3ac0,
    0x3ab0, 0x3aa0, 0x3a88, 0x3a78, 0x3a68, 0x3a58, 0x3a48, 0x3a38,
    0x3a28, 0x3a18, 0x3a08, 0x39f0, 0x39e0, 0x39d0, 0x39c0, 0x39b0,
    0x39a0, 0x3990, 0x3980, 0x3970, 0x3958, 0x3948, 0x3938, 0x3928,
    0x3918, 0x3908, 0x38f8, 0x38e8, 0x38d8, 0x38c8, 0x38b8, 0x38a8,
    0x3890, 0x3880, 0x3870, 0x3860, 0x3850, 0x3840, 0x3830, 0x3820,
    0x3810, 0x3800, 0x37f0, 0x37e0, 0x37d0, 0x37c0, 0x37a8, 0x3798,
    0x3788, 0x3778, 0x3768, 0x3758, 0x3748, 0x3738, 0x3728, 0x3718,
    0x3708, 0x36f8, 0x36e8, 0x36d8, 0x36c8, 0x36b8, 0x36a8, 0x3698,
    0x3688, 0x3678, 0x3668, 0x3658, 0x3648, 0x3630, 0x3620, 0x3610,
    0x3600, 0x35f0, 0x35e0, 0x35d0, 0x35c0, 0x35b0, 0x35a0, 0x3590,
    0x3580, 0x3570, 0x3560, 0x3550, 0x3540, 0x3530, 0x3520, 0x3510,
    0x3500, 0x34f0, 0x34e0, 0x34d0, 0x34c0, 0x34b0, 0x34a0, 0x3490,
    0x3480, 0x3470, 0x3460, 0x3450, 0x3440, 0x3430, 0x3420, 0x3410,
    0x3400, 0x33f0, 0x33e0, 0x33d0, 0x33c8, 0x33b8, 0x33a8, 0x3398,
    0x3388, 0x3378, 0x3368, 0x3358, 0x3348, 0x3338, 0x3328, 0x3318,
    0x3308, 0x32f8, 0x32e8, 0x32d8, 0x32c8, 0x32b8, 0x32a8, 0x3298,
    0x3288, 0x3278, 0x3268, 0x3260, 0x3250, 0x3240, 0x3230, 0x3220,
    0x3210, 0x3200, 0x31f0, 0x31e0, 0x31d0, 0x31c0, 0x31b0, 0x31a0,
    0x3190, 0x3180, 0x3178, 0x3168, 0x3158, 0x3148, 0x3138, 0x3128,
    0x3118, 0x3108, 0x30f8, 0x30e8, 0x30d8, 0x30c8, 0x30c0, 0x30b0,
    0x30a0, 0x3090, 0x3080, 0x3070, 0x3060, 0x3050, 0x3040, 0x3030,
    0x3028, 0x3018, 0x3008, 0x2ff8, 0x2fe8, 0x2fd8, 0x2fc8, 0x2fb8,
    0x2fa8, 0x2fa0, 0x2f90, 0x2f80, 0x2f70, 0x2f60, 0x2f50, 0x2f40,
    0x2f30, 0x2f28, 0x2f18, 0x2f08, 0x2ef8, 0x2ee8, 0x2ed8, 0x2ec8,
    0x2eb8, 0x2eb0, 0x2ea0, 0x2e90, 0x2e80, 0x2e70, 0x2e60, 0x2e50,
    0x2e48, 0x2e38, 0x2e28, 0x2e18, 0x2e08, 0x2df8, 0x2df0, 0x2de0,
    0x2dd0, 0x2dc0, 0x2db0, 0x2da0, 0x2d90, 0x2d88, 0x2d78, 0x2d68,
    0x2d58, 0x2d48, 0x2d38, 0x2d30, 0x2d20, 0x2d10, 0x2d00, 0x2cf0,
    0x2ce0, 0x2cd8, 0x2cc8, 0x2cb8, 0x2ca8, 0x2c98, 0x2c90, 0x2c80,
    0x2c70, 0x2c60, 0x2c50, 0x2c40, 0x2c38, 0x2c28, 0x2c18, 0x2c08,
    0x2bf8, 0x2bf0, 0x2be0, 0x2bd0, 0x2bc0, 0x2bb0, 0x2ba8, 0x2b98,
    0x2b88, 0x2b78, 0x2b68, 0x2b60, 0x2b50, 0x2b40, 0x2b30, 0x2b20,
    0x2b18, 0x2b08, 0x2af8, 0x2ae8, 0x2ae0, 0x2ad0, 0x2ac0, 0x2ab0,
    0x2aa0, 0x2a98, 0x2a88, 0x2a78, 0x2a68, 0x2a60, 0x2a50, 0x2a40,
    0x2a30, 0x2a20, 0x2a18, 0x2a08, 0x29f8, 0x29e8, 0x29e0, 0x29d0,
    0x29c0, 0x29b0, 0x29a8, 0x2998, 0x2988, 0x2978, 0x2970, 0x2960,
    0x2950, 0x2940, 0x2938, 0x2928, 0x2918, 0x2908, 0x2900, 0x28f0,
    0x28e0, 0x28d0, 0x28c8, 0x28b8, 0x28a8, 0x2898, 0x2890, 0x2880,
    0x2870, 0x2868, 0x2858, 0x2848, 0x2838, 0x2830, 0x2820, 0x2810,
    0x2800, 0x27f8, 0x27e8, 0x27d8, 0x27d0, 0x27c0, 0x27b0, 0x27a0,
    0x2798, 0x2788, 0x2778, 0x2770, 0x2760, 0x2750, 0x2740, 0x2738,
    0x2728, 0x2718, 0x2710, 0x2700, 0x26f0, 0x26e8, 0x26d8, 0x26c8,
    0x26b8, 0x26b0, 0x26a0, 0x2690, 0x2688, 0x2678, 0x2668, 0x2660,
    0x2650, 0x2640, 0x2638, 0x2628, 0x2618, 0x2608, 0x2600, 0x25f0,
    0x25e0, 0x25d8, 0x25c8, 0x25b8, 0x25b0, 0x25a0, 0x2590, 0x2588,
    0x2578, 0x2568, 0x2560, 0x2550, 0x2540, 0x2538, 0x2528, 0x2518,
    0x2510, 0x2500, 0x24f0, 0x24e8, 0x24d8, 0x24c8, 0x24c0, 0x24b0,
    0x24a0, 0x2498, 0x2488, 0x2478, 0x2470, 0x2460, 0x2450, 0x2448,
    0x2438, 0x2430, 0x2420, 0x2410, 0x2408, 0x23f8, 0x23e8, 0x23e0,
    0x23d0, 0x23c0, 0x23b8, 0x23a8, 0x23a0, 0x2390, 0x2380, 0x2378,
    0x2368, 0x2358, 0x2350, 0x2340, 0x2330, 0x2328, 0x2318, 0x2310,
    0x2300, 0x22f0, 0x22e8, 0x22d8, 0x22d0, 0x22c0, 0x22b0, 0x22a8,
    0x2298, 0x2288, 0x2280, 0x2270, 0x2268, 0x2258, 0x2248, 0x2240,
    0x2230, 0x2228, 0x2218, 0x2208, 0x2200, 0x21f0, 0x21e8, 0x21d8,
    0x21c8, 0x21c0, 0x21b0, 0x21a8, 0x2198, 0x2188, 0x2180, 0x2170,
    0x2168, 0x2158, 0x2148, 0x2140, 0x2130, 0x2128, 0x2118, 0x2108,
    0x2100, 0x20f0, 0x20e8, 0x20d8, 0x20d0, 0x20c0, 0x20b0, 0x20a8,
    0x2098, 0x2090, 0x2080, 0x2078, 0x2068, 0x2058, 0x2050, 0x2040,
    0x2038, 0x2028, 0x2020, 0x2010, 0x2000, 0x1ff8, 0x1fe8, 0x1fe0,
    0x1fd0, 0x1fc8, 0x1fb8, 0x1fb0, 0x1fa0, 0x1f90, 0x1f88, 0x1f78,
    0x1f70, 0x1f60, 0x1f58, 0x1f48, 0x1f40, 0x1f30, 0x1f20, 0x1f18,
    0x1f08, 0x1f00, 0x1ef0, 0x1ee8, 0x1ed8, 0x1ed0, 0x1ec0, 0x1eb8,
    0x1ea8, 0x1ea0, 0x1e90, 0x1e80, 0x1e78, 0x1e68, 0x1e60, 0x1e50,
    0x1e48, 0x1e38, 0x1e30, 0x1e20, 0x1e18, 0x1e08, 0x1e00, 0x1df0,
    0x1de8, 0x1dd8, 0x1dd0, 0x1dc0, 0x1db8, 0x1da8, 0x1da0, 0x1d90,
    0x1d80, 0x1d78, 0x1d68, 0x1d60, 0x1d50, 0x1d48, 0x1d38, 0x1d30,
    0x1d20, 0x1d18, 0x1d08, 0x1d00, 0x1cf0, 0x1ce8, 0x1cd8, 0x1cd0,
    0x1cc0, 0x1cb8, 0x1ca8, 0x1ca0, 0x1c90, 0x1c88, 0x1c78, 0x1c70,
    0x1c60, 0x1c58, 0x1c48, 0x1c40, 0x1c30, 0x1c28, 0x1c18, 0x1c10,
    0x1c00, 0x1bf8, 0x1bf0, 0x1be0, 0x1bd8, 0x1bc8, 0x1bc0, 0x1bb0,
    0x1ba8, 0x1b98, 0x1b90, 0x1b80, 0x1b78, 0x1b68, 0x1b60, 0x1b50,
    0x1b48, 0x1b38, 0x1b30, 0x1b20, 0x1b18, 0x1b08, 0x1b00, 0x1af8,
    0x1ae8, 0x1ae0, 0x1ad0, 0x1ac8, 0x1ab8, 0x1ab0, 0x1aa0, 0x1a98,
    0x1a88, 0x1a80, 0x1a70, 0x1a68, 0x1a60, 0x1a50, 0x1a48, 0x1a38,
    0x1a30, 0x1a20, 0x1a18, 0x1a08, 0x1a00, 0x19f8, 0x19e8, 0x19e0,
    0x19d0, 0x19c8, 0x19b8, 0x19b0, 0x19a0, 0x1998, 0x1990, 0x1980,
    0x1978, 0x1968, 0x1960, 0x1950, 0x1948, 0x1938, 0x1930, 0x1928,
    0x1918, 0x1910, 0x1900, 0x18f8, 0x18e8, 0x18e0, 0x18d8, 0x18c8,
    0x18c0, 0x18b0, 0x18a8, 0x1898, 0x1890, 0x1888, 0x1878, 0x1870,
    0x1860, 0x1858, 0x1850, 0x1840, 0x1838, 0x1828, 0x1820, 0x1810,
    0x1808, 0x1800, 0x17f0, 0x17e8, 0x17d8, 0x17d0, 0x17c8, 0x17b8,
    0x17b0, 0x17a0, 0x1798, 0x1790, 0x1780, 0x1778, 0x1768, 0x1760,
    0x1758, 0x1748, 0x1740, 0x1730, 0x1728, 0x1720, 0x1710, 0x1708,
    0x16f8, 0x16f0, 0x16e8, 0x16d8, 0x16d0, 0x16c8, 0x16b8, 0x16b0,
    0x16a0, 0x1698, 0x1690, 0x1680, 0x1678, 0x1668, 0x1660, 0x1658,
    0x1648, 0x1640, 0x1638, 0x1628, 0x1620, 0x1610, 0x1608, 0x1600,
    0x15f0, 0x15e8, 0x15e0, 0x15d0, 0x15c8, 0x15b8, 0x15b0, 0x15a8,
    0x1598, 0x1590, 0x1588, 0x1578, 0x1570, 0x1568, 0x1558, 0x1550,
    0x1540, 0x1538, 0x1530, 0x1520, 0x1518, 0x1510, 0x1500, 0x14f8,
    0x14f0, 0x14e0, 0x14d8, 0x14d0, 0x14c0, 0x14b8, 0x14a8, 0x14a0,
    0x1498, 0x1488, 0x1480, 0x1478, 0x1468, 0x1460, 0x1458, 0x1448,
    0x1440, 0x1438, 0x1428, 0x1420, 0x1418, 0x1408, 0x1400, 0x13f8,
    0x13e8, 0x13e0, 0x13d8, 0x13c8, 0x13c0, 0x13b8, 0x13a8, 0x13a0,
    0x1398, 0x1388, 0x1380, 0x1378, 0x1368, 0x1360, 0x1358, 0x1348,
    0x1340, 0x1338, 0x1328, 0x1320, 0x1318, 0x1308, 0x1300, 0x12f8,
    0x12e8, 0x12e0, 0x12d8, 0x12d0, 0x12c0, 0x12b8, 0x12b0, 0x12a0,
    0x1298, 0x1290, 0x1280, 0x1278, 0x1270, 0x1260, 0x1258, 0x1250,
    0x1240, 0x1238, 0x1230, 0x1228, 0x1218, 0x1210, 0x1208, 0x11f8,
    0x11f0, 0x11e8, 0x11d8, 0x11d0, 0x11c8, 0x11c0, 0x11b0, 0x11a8,
    0x11a0, 0x1190, 0x1188, 0x1180, 0x1178, 0x1168, 0x1160, 0x1158,
    0x1148, 0x1140, 0x1138, 0x1128, 0x1120, 0x1118, 0x1110, 0x1100,
    0x10f8, 0x10f0, 0x10e8, 0x10d8, 0x10d0, 0x10c8, 0x10b8, 0x10b0,
    0x10a8, 0x10a0, 0x1090, 0x1088, 0x1080, 0x1070, 0x1068, 0x1060,
    0x1058, 0x1048, 0x1040, 0x1038, 0x1030, 0x1020, 0x1018, 0x1010,
    0x1000, 0x0ff8, 0x0ff0, 0x0fe8, 0x0fd8, 0x0fd0, 0x0fc8, 0x0fc0,
    0x0fb0, 0x0fa8, 0x0fa0, 0x0f98, 0x0f88, 0x0f80, 0x0f78, 0x0f70,
    0x0f60, 0x0f58, 0x0f50, 0x0f48, 0x0f38, 0x0f30, 0x0f28, 0x0f20,
    0x0f10, 0x0f08, 0x0f00, 0x0ef8, 0x0ee8, 0x0ee0, 0x0ed8, 0x0ed0,
    0x0ec0, 0x0eb8, 0x0eb0, 0x0ea8, 0x0e98, 0x0e90, 0x0e88, 0x0e80,
    0x0e70, 0x0e68, 0x0e60, 0x0e58, 0x0e48, 0x0e40, 0x0e38, 0x0e30,
    0x0e28, 0x0e18, 0x0e10, 0x0e08, 0x0e00, 0x0df0, 0x0de8, 0x0de0,
    0x0dd8, 0x0dc8, 0x0dc0, 0x0db8, 0x0db0, 0x0da8, 0x0d98, 0x0d90,
    0x0d88, 0x0d80, 0x0d70, 0x0d68, 0x0d60, 0x0d58, 0x0d50, 0x0d40,
    0x0d38, 0x0d30, 0x0d28, 0x0d18, 0x0d10, 0x0d08, 0x0d00, 0x0cf8,
    0x0ce8, 0x0ce0, 0x0cd8, 0x0cd0, 0x0cc8, 0x0cb8, 0x0cb0, 0x0ca8,
    0x0ca0, 0x0c98, 0x0c88, 0x0c80, 0x0c78, 0x0c70, 0x0c60, 0x0c58,
    0x0c50, 0x0c48, 0x0c40, 0x0c30, 0x0c28, 0x0c20, 0x0c18, 0x0c10,
    0x0c00, 0x0bf8, 0x0bf0, 0x0be8, 0x0be0, 0x0bd8, 0x0bc8, 0x0bc0,
    0x0bb8, 0x0bb0, 0x0ba8, 0x0b98, 0x0b90, 0x0b88, 0x0b80, 0x0b78,
    0x0b68, 0x0b60, 0x0b58, 0x0b50, 0x0b48, 0x0b40, 0x0b30, 0x0b28,
    0x0b20, 0x0b18, 0x0b10, 0x0b00, 0x0af8, 0x0af0, 0x0ae8, 0x0ae0,
    0x0ad8, 0x0ac8, 0x0ac0, 0x0ab8, 0x0ab0, 0x0aa8, 0x0a98, 0x0a90,
    0x0a88, 0x0a80, 0x0a78, 0x0a70, 0x0a60, 0x0a58, 0x0a50, 0x0a48,
    0x0a40, 0x0a38, 0x0a28, 0x0a20, 0x0a18, 0x0a10, 0x0a08, 0x0a00,
    0x09f0, 0x09e8, 0x09e0, 0x09d8, 0x09d0, 0x09c8, 0x09c0, 0x09b0,
    0x09a8, 0x09a0, 0x0998, 0x0990, 0x0988, 0x0978, 0x0970, 0x0968,
    0x0960, 0x0958, 0x0950, 0x0948, 0x0938, 0x0930, 0x0928, 0x0920,
    0x0918, 0x0910, 0x0900, 0x08f8, 0x08f0, 0x08e8, 0x08e0, 0x08d8,
    0x08d0, 0x08c0, 0x08b8, 0x08b0, 0x08a8, 0x08a0, 0x0898, 0x0890,
    0x0880, 0x0878, 0x0870, 0x0868, 0x0860, 0x0858, 0x0850, 0x0848,
    0x0838, 0x0830, 0x0828, 0x0820, 0x0818, 0x0810, 0x0808, 0x0800,
    0x07f0, 0x07e8, 0x07e0, 0x07d8, 0x07d0, 0x07c8, 0x07c0, 0x07b0,
    0x07a8, 0x07a0, 0x0798, 0x0790, 0x0788, 0x0780, 0x0778, 0x0770,
    0x0760, 0x0758, 0x0750, 0x0748, 0x0740, 0x0738, 0x0730, 0x0728,
    0x0718, 0x0710, 0x0708, 0x0700, 0x06f8, 0x06f0, 0x06e8, 0x06e0,
    0x06d8, 0x06c8, 0x06c0, 0x06b8, 0x06b0, 0x06a8, 0x06a0, 0x0698,
    0x0690, 0x0688, 0x0680, 0x0670, 0x0668, 0x0660, 0x0658, 0x0650,
    0x0648, 0x0640, 0x0638, 0x0630, 0x0620, 0x0618, 0x0610, 0x0608,
    0x0600, 0x05f8, 0x05f0, 0x05e8, 0x05e0, 0x05d8, 0x05d0, 0x05c0,
    0x05b8, 0x05b0, 0x05a8, 0x05a0, 0x0598, 0x0590, 0x0588, 0x0580,
    0x0578, 0x0570, 0x0560, 0x0558, 0x0550, 0x0548, 0x0540, 0x0538,
    0x0530, 0x0528, 0x0520, 0x0518, 0x0510, 0x0508, 0x04f8, 0x04f0,
    0x04e8, 0x04e0, 0x04d8, 0x04d0, 0x04c8, 0x04c0, 0x04b8, 0x04b0,
    0x04a8, 0x04a0, 0x0498, 0x0488, 0x0480, 0x0478, 0x0470, 0x0468,
    0x0460, 0x0458, 0x0450, 0x0448, 0x0440, 0x0438, 0x0430, 0x0428,
    0x0420, 0x0418, 0x0408, 0x0400, 0x03f8, 0x03f0, 0x03e8, 0x03e0,
    0x03d8, 0x03d0, 0x03c8, 0x03c0, 0x03b8, 0x03b0, 0x03a8, 0x03a0,
    0x0398, 0x0390, 0x0388, 0x0378, 0x0370, 0x0368, 0x0360, 0x0358,
    0x0350, 0x0348, 0x0340, 0x0338, 0x0330, 0x0328, 0x0320, 0x0318,
    0x0310, 0x0308, 0x0300, 0x02f8, 0x02f0, 0x02e8, 0x02d8, 0x02d0,
    0x02c8, 0x02c0, 0x02b8, 0x02b0, 0x02a8, 0x02a0, 0x0298, 0x0290,
    0x0288, 0x0280, 0x0278, 0x0270, 0x0268, 0x0260, 0x0258, 0x0250,
    0x0248, 0x0240, 0x0238, 0x0230, 0x0228, 0x0220, 0x0218, 0x0210,
    0x0200, 0x01f8, 0x01f0, 0x01e8, 0x01e0, 0x01d8, 0x01d0, 0x01c8,
    0x01c0, 0x01b8, 0x01b0, 0x01a8, 0x01a0, 0x0198, 0x0190, 0x0188,
    0x0180, 0x0178, 0x0170, 0x0168, 0x0160, 0x0158, 0x0150, 0x0148,
    0x0140, 0x0138, 0x0130, 0x0128, 0x0120, 0x0118, 0x0110, 0x0108,
    0x0100, 0x00f8, 0x00f0, 0x00e8, 0x00e0, 0x00d8, 0x00d0, 0x00c8,
    0x00c0, 0x00b8, 0x00b0, 0x00a8, 0x00a0, 0x0098, 0x0090, 0x0088,
    0x0080, 0x0078, 0x0070, 0x0068, 0x0060, 0x0058, 0x0050, 0x0048,
    0x0040, 0x0038, 0x0030, 0x0028, 0x0020, 0x0018, 0x0010, 0x0008
};

// approximate reciprocal of scalar single precision FP
static float32 approximate_rcp(float32 op)
{
  softfloat_class_t op_class = f32_class(op);
  bool sign = f32_sign(op);

  switch(op_class) {
    case softfloat_zero:
    case softfloat_denormal:
      return packFloat32(sign, 0xFF, 0);

    case softfloat_negative_inf:
    case softfloat_positive_inf:
      return packFloat32(sign, 0, 0);

    case softfloat_SNaN:
    case softfloat_QNaN:
      return f32_convert_to_QNaN(op);

    case softfloat_normalized:
      break;
  }

  uint32_t fraction = f32_fraction(op);
  int16_t exp = f32_exp(op);

  /*
   * Calculate (1/1.yyyyyyyyyyy1), the result is always rounded to the
   *  12th bit after the decimal point by round-to-nearest, regardless
   *  of the current rounding mode.
   *
   * Using precalculated 2048-entry table.
   */

  exp = 2 * FLOAT32_EXP_BIAS - 1 - exp;

  /* check for underflow */
  if (exp <= 0)
      return packFloat32(sign, 0, 0);

  return packFloat32(sign, exp, (uint32_t)(rcp_table[fraction >> 12]) << 8);
}

static int
opRCPSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rcp(src.f[0]);

    return 0;
}

static int
opRCPSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rcp(src.f[0]);

    return 0;
}

static int
opRCPPS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rcp(src.f[0]);
    cpu_state.XMM[cpu_reg].f[1] = approximate_rcp(src.f[1]);
    cpu_state.XMM[cpu_reg].f[2] = approximate_rcp(src.f[2]);
    cpu_state.XMM[cpu_reg].f[3] = approximate_rcp(src.f[3]);

    return 0;
}

static int
opRCPPS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = approximate_rcp(src.f[0]);
    cpu_state.XMM[cpu_reg].f[1] = approximate_rcp(src.f[1]);
    cpu_state.XMM[cpu_reg].f[2] = approximate_rcp(src.f[2]);
    cpu_state.XMM[cpu_reg].f[3] = approximate_rcp(src.f[3]);

    return 0;
}

static int
opADDPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opADDPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_add(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_add(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_add(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_add(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }
    
    return 0;
}

static int
opADDPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opADDPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_add(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_add(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_add(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_add(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opADDSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_add(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opADDSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_add(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMULPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMULPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_mul(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_mul(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_mul(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_mul(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMULPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMULPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_mul(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_mul(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_mul(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_mul(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMULSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_mul(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMULSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_mul(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opSUBPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSUBPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sub(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_sub(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_sub(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_sub(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opSUBPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opSUBPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sub(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_sub(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_sub(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_sub(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opSUBSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sub(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opSUBSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_sub(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMINPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMINPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_min(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_min(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_min(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_min(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMINPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMINPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_min(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_min(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_min(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_min(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMINSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_min(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMINSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_min(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }


    return 0;
}

static int
opDIVPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opDIVPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_div(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_div(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_div(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_div(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opDIVPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opDIVPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_div(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_div(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_div(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_div(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opDIVSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_div(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opDIVSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_div(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMAXPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMAXPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_max(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_max(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_max(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_max(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMAXPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opMAXPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_max(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    cpu_state.XMM[cpu_reg].f[1] = f32_max(cpu_state.XMM[cpu_reg].f[1], src.f[1], &status);
    cpu_state.XMM[cpu_reg].f[2] = f32_max(cpu_state.XMM[cpu_reg].f[2], src.f[2], &status);
    cpu_state.XMM[cpu_reg].f[3] = f32_max(cpu_state.XMM[cpu_reg].f[3], src.f[3], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMAXSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_max(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

static int
opMAXSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    cpu_state.XMM[cpu_reg].f[0] = f32_max(cpu_state.XMM[cpu_reg].f[0], src.f[0], &status);
    softfloat_status_word_to_mxcsr(status);
    int unmasked = (~cpu_state.mxcsr >> 7) & 0x3f;
    if ((cpu_state.mxcsr & 0x3f) & (unmasked & 0x3f)) {
        if (cr4 & CR4_OSXMMEXCPT)
            x86_int(0x13);
        ILLEGAL_ON(!(cr4 & CR4_OSXMMEXCPT));
    }

    return 0;
}

/* Comparison predicate for CMPSS/CMPPS instructions */
static float32_compare_method compare32[8] = {
  f32_eq_ordered_quiet,
  f32_lt_ordered_signalling,
  f32_le_ordered_signalling,
  f32_unordered_quiet,
  f32_neq_unordered_quiet,
  f32_nlt_unordered_signalling,
  f32_nle_unordered_signalling,
  f32_ordered_quiet
};

static int
opCMPPS_xmm_xmm_a16(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opCMPPD_xmm_xmm_a16(fetchdat);

    SSE_ENTER();

    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    uint8_t  imm = getbyte();
    cpu_state.XMM[cpu_reg].l[0] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[0], src.f[0], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[1] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[1], src.f[1], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[2] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[2], src.f[2], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[3] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[3], src.f[3], &status) ? ~0 : 0;

    return 0;
}

static int
opCMPPS_xmm_xmm_a32(uint32_t fetchdat)
{
    if(cpu_state.sse_xmm && (cpu_has_feature(CPU_FEATURE_SSE2)))
        return opCMPPD_xmm_xmm_a32(fetchdat);

    SSE_ENTER();

    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    uint8_t  imm = getbyte();
    cpu_state.XMM[cpu_reg].l[0] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[0], src.f[0], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[1] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[1], src.f[1], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[2] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[2], src.f[2], &status) ? ~0 : 0;
    cpu_state.XMM[cpu_reg].l[3] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[3], src.f[3], &status) ? ~0 : 0;
    return 0;
}

static int
opCMPSS_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    uint8_t  imm = getbyte();
    cpu_state.XMM[cpu_reg].l[0] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[0], src.f[0], &status) ? ~0 : 0;

    return 0;
}

static int
opCMPSS_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    SSE_REG src;
    struct softfloat_status_t status = mxcsr_to_softfloat_status_word();
    SSE_GETSRC();
    uint8_t  imm = getbyte();
    cpu_state.XMM[cpu_reg].l[0] = compare32[imm & 7](cpu_state.XMM[cpu_reg].f[0], src.f[0], &status) ? ~0 : 0;

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

    if(src.b[0] < dst->b[0]) dst->b[0] = src.b[0];
    if(src.b[1] < dst->b[1]) dst->b[1] = src.b[1];
    if(src.b[2] < dst->b[2]) dst->b[2] = src.b[2];
    if(src.b[3] < dst->b[3]) dst->b[3] = src.b[3];
    if(src.b[4] < dst->b[4]) dst->b[4] = src.b[4];
    if(src.b[5] < dst->b[5]) dst->b[5] = src.b[5];
    if(src.b[6] < dst->b[6]) dst->b[6] = src.b[6];
    if(src.b[7] < dst->b[7]) dst->b[7] = src.b[7];

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

    if(src.b[0] < dst->b[0]) dst->b[0] = src.b[0];
    if(src.b[1] < dst->b[1]) dst->b[1] = src.b[1];
    if(src.b[2] < dst->b[2]) dst->b[2] = src.b[2];
    if(src.b[3] < dst->b[3]) dst->b[3] = src.b[3];
    if(src.b[4] < dst->b[4]) dst->b[4] = src.b[4];
    if(src.b[5] < dst->b[5]) dst->b[5] = src.b[5];
    if(src.b[6] < dst->b[6]) dst->b[6] = src.b[6];
    if(src.b[7] < dst->b[7]) dst->b[7] = src.b[7];

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMINUB_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    SSE_REG src;
    fetch_ea_16(fetchdat);
    SSE_GETSRC();

    if(src.b[0] < cpu_state.XMM[cpu_reg].b[0]) cpu_state.XMM[cpu_reg].b[0] = src.b[0];
    if(src.b[1] < cpu_state.XMM[cpu_reg].b[1]) cpu_state.XMM[cpu_reg].b[1] = src.b[1];
    if(src.b[2] < cpu_state.XMM[cpu_reg].b[2]) cpu_state.XMM[cpu_reg].b[2] = src.b[2];
    if(src.b[3] < cpu_state.XMM[cpu_reg].b[3]) cpu_state.XMM[cpu_reg].b[3] = src.b[3];
    if(src.b[4] < cpu_state.XMM[cpu_reg].b[4]) cpu_state.XMM[cpu_reg].b[4] = src.b[4];
    if(src.b[5] < cpu_state.XMM[cpu_reg].b[5]) cpu_state.XMM[cpu_reg].b[5] = src.b[5];
    if(src.b[6] < cpu_state.XMM[cpu_reg].b[6]) cpu_state.XMM[cpu_reg].b[6] = src.b[6];
    if(src.b[7] < cpu_state.XMM[cpu_reg].b[7]) cpu_state.XMM[cpu_reg].b[7] = src.b[7];
    if(src.b[8] < cpu_state.XMM[cpu_reg].b[8]) cpu_state.XMM[cpu_reg].b[8] = src.b[8];
    if(src.b[9] < cpu_state.XMM[cpu_reg].b[9]) cpu_state.XMM[cpu_reg].b[9] = src.b[9];
    if(src.b[10] < cpu_state.XMM[cpu_reg].b[10]) cpu_state.XMM[cpu_reg].b[10] = src.b[10];
    if(src.b[11] < cpu_state.XMM[cpu_reg].b[11]) cpu_state.XMM[cpu_reg].b[11] = src.b[11];
    if(src.b[12] < cpu_state.XMM[cpu_reg].b[12]) cpu_state.XMM[cpu_reg].b[12] = src.b[12];
    if(src.b[13] < cpu_state.XMM[cpu_reg].b[13]) cpu_state.XMM[cpu_reg].b[13] = src.b[13];
    if(src.b[14] < cpu_state.XMM[cpu_reg].b[14]) cpu_state.XMM[cpu_reg].b[14] = src.b[14];
    if(src.b[15] < cpu_state.XMM[cpu_reg].b[15]) cpu_state.XMM[cpu_reg].b[15] = src.b[15];

    return 0;
}

static int
opPMINUB_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    SSE_REG src;
    fetch_ea_32(fetchdat);
    SSE_GETSRC();

    if(src.b[0] < cpu_state.XMM[cpu_reg].b[0]) cpu_state.XMM[cpu_reg].b[0] = src.b[0];
    if(src.b[1] < cpu_state.XMM[cpu_reg].b[1]) cpu_state.XMM[cpu_reg].b[1] = src.b[1];
    if(src.b[2] < cpu_state.XMM[cpu_reg].b[2]) cpu_state.XMM[cpu_reg].b[2] = src.b[2];
    if(src.b[3] < cpu_state.XMM[cpu_reg].b[3]) cpu_state.XMM[cpu_reg].b[3] = src.b[3];
    if(src.b[4] < cpu_state.XMM[cpu_reg].b[4]) cpu_state.XMM[cpu_reg].b[4] = src.b[4];
    if(src.b[5] < cpu_state.XMM[cpu_reg].b[5]) cpu_state.XMM[cpu_reg].b[5] = src.b[5];
    if(src.b[6] < cpu_state.XMM[cpu_reg].b[6]) cpu_state.XMM[cpu_reg].b[6] = src.b[6];
    if(src.b[7] < cpu_state.XMM[cpu_reg].b[7]) cpu_state.XMM[cpu_reg].b[7] = src.b[7];
    if(src.b[8] < cpu_state.XMM[cpu_reg].b[8]) cpu_state.XMM[cpu_reg].b[8] = src.b[8];
    if(src.b[9] < cpu_state.XMM[cpu_reg].b[9]) cpu_state.XMM[cpu_reg].b[9] = src.b[9];
    if(src.b[10] < cpu_state.XMM[cpu_reg].b[10]) cpu_state.XMM[cpu_reg].b[10] = src.b[10];
    if(src.b[11] < cpu_state.XMM[cpu_reg].b[11]) cpu_state.XMM[cpu_reg].b[11] = src.b[11];
    if(src.b[12] < cpu_state.XMM[cpu_reg].b[12]) cpu_state.XMM[cpu_reg].b[12] = src.b[12];
    if(src.b[13] < cpu_state.XMM[cpu_reg].b[13]) cpu_state.XMM[cpu_reg].b[13] = src.b[13];
    if(src.b[14] < cpu_state.XMM[cpu_reg].b[14]) cpu_state.XMM[cpu_reg].b[14] = src.b[14];
    if(src.b[15] < cpu_state.XMM[cpu_reg].b[15]) cpu_state.XMM[cpu_reg].b[15] = src.b[15];

    return 0;
}

static int
opPMINUB_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPMINUB_xmm_xmm_a16(fetchdat);
    else
        return opPMINUB_mm_mm_a16(fetchdat);
}

static int
opPMINUB_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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

    if(src.b[0] > dst->b[0]) dst->b[0] = src.b[0];
    if(src.b[1] > dst->b[1]) dst->b[1] = src.b[1];
    if(src.b[2] > dst->b[2]) dst->b[2] = src.b[2];
    if(src.b[3] > dst->b[3]) dst->b[3] = src.b[3];
    if(src.b[4] > dst->b[4]) dst->b[4] = src.b[4];
    if(src.b[5] > dst->b[5]) dst->b[5] = src.b[5];
    if(src.b[6] > dst->b[6]) dst->b[6] = src.b[6];
    if(src.b[7] > dst->b[7]) dst->b[7] = src.b[7];

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

    if(src.b[0] > dst->b[0]) dst->b[0] = src.b[0];
    if(src.b[1] > dst->b[1]) dst->b[1] = src.b[1];
    if(src.b[2] > dst->b[2]) dst->b[2] = src.b[2];
    if(src.b[3] > dst->b[3]) dst->b[3] = src.b[3];
    if(src.b[4] > dst->b[4]) dst->b[4] = src.b[4];
    if(src.b[5] > dst->b[5]) dst->b[5] = src.b[5];
    if(src.b[6] > dst->b[6]) dst->b[6] = src.b[6];
    if(src.b[7] > dst->b[7]) dst->b[7] = src.b[7];

    MMX_SETEXP(cpu_reg);
    CLOCK_CYCLES(1);

    return 0;
}

static int
opPMAXUB_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    SSE_REG src;
    fetch_ea_16(fetchdat);
    SSE_GETSRC();
    
    if(src.b[0] > cpu_state.XMM[cpu_reg].b[0]) cpu_state.XMM[cpu_reg].b[0] = src.b[0];
    if(src.b[1] > cpu_state.XMM[cpu_reg].b[1]) cpu_state.XMM[cpu_reg].b[1] = src.b[1];
    if(src.b[2] > cpu_state.XMM[cpu_reg].b[2]) cpu_state.XMM[cpu_reg].b[2] = src.b[2];
    if(src.b[3] > cpu_state.XMM[cpu_reg].b[3]) cpu_state.XMM[cpu_reg].b[3] = src.b[3];
    if(src.b[4] > cpu_state.XMM[cpu_reg].b[4]) cpu_state.XMM[cpu_reg].b[4] = src.b[4];
    if(src.b[5] > cpu_state.XMM[cpu_reg].b[5]) cpu_state.XMM[cpu_reg].b[5] = src.b[5];
    if(src.b[6] > cpu_state.XMM[cpu_reg].b[6]) cpu_state.XMM[cpu_reg].b[6] = src.b[6];
    if(src.b[7] > cpu_state.XMM[cpu_reg].b[7]) cpu_state.XMM[cpu_reg].b[7] = src.b[7];
    if(src.b[8] > cpu_state.XMM[cpu_reg].b[8]) cpu_state.XMM[cpu_reg].b[8] = src.b[8];
    if(src.b[9] > cpu_state.XMM[cpu_reg].b[9]) cpu_state.XMM[cpu_reg].b[9] = src.b[9];
    if(src.b[10] > cpu_state.XMM[cpu_reg].b[10]) cpu_state.XMM[cpu_reg].b[10] = src.b[10];
    if(src.b[11] > cpu_state.XMM[cpu_reg].b[11]) cpu_state.XMM[cpu_reg].b[11] = src.b[11];
    if(src.b[12] > cpu_state.XMM[cpu_reg].b[12]) cpu_state.XMM[cpu_reg].b[12] = src.b[12];
    if(src.b[13] > cpu_state.XMM[cpu_reg].b[13]) cpu_state.XMM[cpu_reg].b[13] = src.b[13];
    if(src.b[14] > cpu_state.XMM[cpu_reg].b[14]) cpu_state.XMM[cpu_reg].b[14] = src.b[14];
    if(src.b[15] > cpu_state.XMM[cpu_reg].b[15]) cpu_state.XMM[cpu_reg].b[15] = src.b[15];

    return 0;
}

static int
opPMAXUB_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    SSE_REG src;
    fetch_ea_32(fetchdat);
    SSE_GETSRC();

    if(src.b[0] > cpu_state.XMM[cpu_reg].b[0]) cpu_state.XMM[cpu_reg].b[0] = src.b[0];
    if(src.b[1] > cpu_state.XMM[cpu_reg].b[1]) cpu_state.XMM[cpu_reg].b[1] = src.b[1];
    if(src.b[2] > cpu_state.XMM[cpu_reg].b[2]) cpu_state.XMM[cpu_reg].b[2] = src.b[2];
    if(src.b[3] > cpu_state.XMM[cpu_reg].b[3]) cpu_state.XMM[cpu_reg].b[3] = src.b[3];
    if(src.b[4] > cpu_state.XMM[cpu_reg].b[4]) cpu_state.XMM[cpu_reg].b[4] = src.b[4];
    if(src.b[5] > cpu_state.XMM[cpu_reg].b[5]) cpu_state.XMM[cpu_reg].b[5] = src.b[5];
    if(src.b[6] > cpu_state.XMM[cpu_reg].b[6]) cpu_state.XMM[cpu_reg].b[6] = src.b[6];
    if(src.b[7] > cpu_state.XMM[cpu_reg].b[7]) cpu_state.XMM[cpu_reg].b[7] = src.b[7];
    if(src.b[8] > cpu_state.XMM[cpu_reg].b[8]) cpu_state.XMM[cpu_reg].b[8] = src.b[8];
    if(src.b[9] > cpu_state.XMM[cpu_reg].b[9]) cpu_state.XMM[cpu_reg].b[9] = src.b[9];
    if(src.b[10] > cpu_state.XMM[cpu_reg].b[10]) cpu_state.XMM[cpu_reg].b[10] = src.b[10];
    if(src.b[11] > cpu_state.XMM[cpu_reg].b[11]) cpu_state.XMM[cpu_reg].b[11] = src.b[11];
    if(src.b[12] > cpu_state.XMM[cpu_reg].b[12]) cpu_state.XMM[cpu_reg].b[12] = src.b[12];
    if(src.b[13] > cpu_state.XMM[cpu_reg].b[13]) cpu_state.XMM[cpu_reg].b[13] = src.b[13];
    if(src.b[14] > cpu_state.XMM[cpu_reg].b[14]) cpu_state.XMM[cpu_reg].b[14] = src.b[14];
    if(src.b[15] > cpu_state.XMM[cpu_reg].b[15]) cpu_state.XMM[cpu_reg].b[15] = src.b[15];

    return 0;
}

static int
opPMAXUB_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPMAXUB_xmm_xmm_a16(fetchdat);
    else
        return opPMAXUB_mm_mm_a16(fetchdat);
}

static int
opPMAXUB_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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
    SSE_ENTER();
    uint16_t tmp[16];
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        tmp[0]  = cpu_state.XMM[cpu_reg].b[0] + cpu_state.XMM[cpu_rm].b[0] + 1;
        tmp[1]  = cpu_state.XMM[cpu_reg].b[1] + cpu_state.XMM[cpu_rm].b[1] + 1;
        tmp[2]  = cpu_state.XMM[cpu_reg].b[2] + cpu_state.XMM[cpu_rm].b[2] + 1;
        tmp[3]  = cpu_state.XMM[cpu_reg].b[3] + cpu_state.XMM[cpu_rm].b[3] + 1;
        tmp[4]  = cpu_state.XMM[cpu_reg].b[4] + cpu_state.XMM[cpu_rm].b[4] + 1;
        tmp[5]  = cpu_state.XMM[cpu_reg].b[5] + cpu_state.XMM[cpu_rm].b[5] + 1;
        tmp[6]  = cpu_state.XMM[cpu_reg].b[6] + cpu_state.XMM[cpu_rm].b[6] + 1;
        tmp[7]  = cpu_state.XMM[cpu_reg].b[7] + cpu_state.XMM[cpu_rm].b[7] + 1;
        tmp[8]  = cpu_state.XMM[cpu_reg].b[8] + cpu_state.XMM[cpu_rm].b[8] + 1;
        tmp[9]  = cpu_state.XMM[cpu_reg].b[9] + cpu_state.XMM[cpu_rm].b[9] + 1;
        tmp[10] = cpu_state.XMM[cpu_reg].b[10] + cpu_state.XMM[cpu_rm].b[10] + 1;
        tmp[11] = cpu_state.XMM[cpu_reg].b[11] + cpu_state.XMM[cpu_rm].b[11] + 1;
        tmp[12] = cpu_state.XMM[cpu_reg].b[12] + cpu_state.XMM[cpu_rm].b[12] + 1;
        tmp[13] = cpu_state.XMM[cpu_reg].b[13] + cpu_state.XMM[cpu_rm].b[13] + 1;
        tmp[14] = cpu_state.XMM[cpu_reg].b[14] + cpu_state.XMM[cpu_rm].b[14] + 1;
        tmp[15] = cpu_state.XMM[cpu_reg].b[15] + cpu_state.XMM[cpu_rm].b[15] + 1;

        cpu_state.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
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

        tmp[0]  = cpu_state.XMM[cpu_reg].b[0] + src[0] + 1;
        tmp[1]  = cpu_state.XMM[cpu_reg].b[1] + src[1] + 1;
        tmp[2]  = cpu_state.XMM[cpu_reg].b[2] + src[2] + 1;
        tmp[3]  = cpu_state.XMM[cpu_reg].b[3] + src[3] + 1;
        tmp[4]  = cpu_state.XMM[cpu_reg].b[4] + src[4] + 1;
        tmp[5]  = cpu_state.XMM[cpu_reg].b[5] + src[5] + 1;
        tmp[6]  = cpu_state.XMM[cpu_reg].b[6] + src[6] + 1;
        tmp[7]  = cpu_state.XMM[cpu_reg].b[7] + src[7] + 1;
        tmp[8]  = cpu_state.XMM[cpu_reg].b[8] + src[8] + 1;
        tmp[9]  = cpu_state.XMM[cpu_reg].b[9] + src[9] + 1;
        tmp[10] = cpu_state.XMM[cpu_reg].b[10] + src[10] + 1;
        tmp[11] = cpu_state.XMM[cpu_reg].b[11] + src[11] + 1;
        tmp[12] = cpu_state.XMM[cpu_reg].b[12] + src[12] + 1;
        tmp[13] = cpu_state.XMM[cpu_reg].b[13] + src[13] + 1;
        tmp[14] = cpu_state.XMM[cpu_reg].b[14] + src[14] + 1;
        tmp[15] = cpu_state.XMM[cpu_reg].b[15] + src[15] + 1;

        cpu_state.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
    }

    return 0;
}

static int
opPAVGB_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    uint16_t tmp[16];
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        tmp[0]  = cpu_state.XMM[cpu_reg].b[0] + cpu_state.XMM[cpu_rm].b[0] + 1;
        tmp[1]  = cpu_state.XMM[cpu_reg].b[1] + cpu_state.XMM[cpu_rm].b[1] + 1;
        tmp[2]  = cpu_state.XMM[cpu_reg].b[2] + cpu_state.XMM[cpu_rm].b[2] + 1;
        tmp[3]  = cpu_state.XMM[cpu_reg].b[3] + cpu_state.XMM[cpu_rm].b[3] + 1;
        tmp[4]  = cpu_state.XMM[cpu_reg].b[4] + cpu_state.XMM[cpu_rm].b[4] + 1;
        tmp[5]  = cpu_state.XMM[cpu_reg].b[5] + cpu_state.XMM[cpu_rm].b[5] + 1;
        tmp[6]  = cpu_state.XMM[cpu_reg].b[6] + cpu_state.XMM[cpu_rm].b[6] + 1;
        tmp[7]  = cpu_state.XMM[cpu_reg].b[7] + cpu_state.XMM[cpu_rm].b[7] + 1;
        tmp[8]  = cpu_state.XMM[cpu_reg].b[8] + cpu_state.XMM[cpu_rm].b[8] + 1;
        tmp[9]  = cpu_state.XMM[cpu_reg].b[9] + cpu_state.XMM[cpu_rm].b[9] + 1;
        tmp[10] = cpu_state.XMM[cpu_reg].b[10] + cpu_state.XMM[cpu_rm].b[10] + 1;
        tmp[11] = cpu_state.XMM[cpu_reg].b[11] + cpu_state.XMM[cpu_rm].b[11] + 1;
        tmp[12] = cpu_state.XMM[cpu_reg].b[12] + cpu_state.XMM[cpu_rm].b[12] + 1;
        tmp[13] = cpu_state.XMM[cpu_reg].b[13] + cpu_state.XMM[cpu_rm].b[13] + 1;
        tmp[14] = cpu_state.XMM[cpu_reg].b[14] + cpu_state.XMM[cpu_rm].b[14] + 1;
        tmp[15] = cpu_state.XMM[cpu_reg].b[15] + cpu_state.XMM[cpu_rm].b[15] + 1;

        cpu_state.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
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

        tmp[0]  = cpu_state.XMM[cpu_reg].b[0] + src[0] + 1;
        tmp[1]  = cpu_state.XMM[cpu_reg].b[1] + src[1] + 1;
        tmp[2]  = cpu_state.XMM[cpu_reg].b[2] + src[2] + 1;
        tmp[3]  = cpu_state.XMM[cpu_reg].b[3] + src[3] + 1;
        tmp[4]  = cpu_state.XMM[cpu_reg].b[4] + src[4] + 1;
        tmp[5]  = cpu_state.XMM[cpu_reg].b[5] + src[5] + 1;
        tmp[6]  = cpu_state.XMM[cpu_reg].b[6] + src[6] + 1;
        tmp[7]  = cpu_state.XMM[cpu_reg].b[7] + src[7] + 1;
        tmp[8]  = cpu_state.XMM[cpu_reg].b[8] + src[8] + 1;
        tmp[9]  = cpu_state.XMM[cpu_reg].b[9] + src[9] + 1;
        tmp[10] = cpu_state.XMM[cpu_reg].b[10] + src[10] + 1;
        tmp[11] = cpu_state.XMM[cpu_reg].b[11] + src[11] + 1;
        tmp[12] = cpu_state.XMM[cpu_reg].b[12] + src[12] + 1;
        tmp[13] = cpu_state.XMM[cpu_reg].b[13] + src[13] + 1;
        tmp[14] = cpu_state.XMM[cpu_reg].b[14] + src[14] + 1;
        tmp[15] = cpu_state.XMM[cpu_reg].b[15] + src[15] + 1;

        cpu_state.XMM[cpu_reg].b[0]  = (uint8_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].b[1]  = (uint8_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].b[2]  = (uint8_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].b[3]  = (uint8_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].b[4]  = (uint8_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].b[5]  = (uint8_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].b[6]  = (uint8_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].b[7]  = (uint8_t) (tmp[7] >> 1);
        cpu_state.XMM[cpu_reg].b[8]  = (uint8_t) (tmp[8] >> 1);
        cpu_state.XMM[cpu_reg].b[9]  = (uint8_t) (tmp[9] >> 1);
        cpu_state.XMM[cpu_reg].b[10] = (uint8_t) (tmp[10] >> 1);
        cpu_state.XMM[cpu_reg].b[11] = (uint8_t) (tmp[11] >> 1);
        cpu_state.XMM[cpu_reg].b[12] = (uint8_t) (tmp[12] >> 1);
        cpu_state.XMM[cpu_reg].b[13] = (uint8_t) (tmp[13] >> 1);
        cpu_state.XMM[cpu_reg].b[14] = (uint8_t) (tmp[14] >> 1);
        cpu_state.XMM[cpu_reg].b[15] = (uint8_t) (tmp[15] >> 1);
    }

    return 0;
}

static int
opPAVGB_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPAVGB_xmm_xmm_a16(fetchdat);
    else
        return opPAVGB_mm_mm_a16(fetchdat);
}

static int
opPAVGB_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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
    SSE_ENTER();
    uint32_t tmp[8];
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        tmp[0] = cpu_state.XMM[cpu_reg].w[0] + cpu_state.XMM[cpu_rm].w[0] + 1;
        tmp[1] = cpu_state.XMM[cpu_reg].w[1] + cpu_state.XMM[cpu_rm].w[1] + 1;
        tmp[2] = cpu_state.XMM[cpu_reg].w[2] + cpu_state.XMM[cpu_rm].w[2] + 1;
        tmp[3] = cpu_state.XMM[cpu_reg].w[3] + cpu_state.XMM[cpu_rm].w[3] + 1;
        tmp[4] = cpu_state.XMM[cpu_reg].w[4] + cpu_state.XMM[cpu_rm].w[4] + 1;
        tmp[5] = cpu_state.XMM[cpu_reg].w[5] + cpu_state.XMM[cpu_rm].w[5] + 1;
        tmp[6] = cpu_state.XMM[cpu_reg].w[6] + cpu_state.XMM[cpu_rm].w[6] + 1;
        tmp[7] = cpu_state.XMM[cpu_reg].w[7] + cpu_state.XMM[cpu_rm].w[7] + 1;

        cpu_state.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
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

        tmp[0] = cpu_state.XMM[cpu_reg].w[0] + src[0] + 1;
        tmp[1] = cpu_state.XMM[cpu_reg].w[1] + src[1] + 1;
        tmp[2] = cpu_state.XMM[cpu_reg].w[2] + src[2] + 1;
        tmp[3] = cpu_state.XMM[cpu_reg].w[3] + src[3] + 1;
        tmp[4] = cpu_state.XMM[cpu_reg].w[4] + src[4] + 1;
        tmp[5] = cpu_state.XMM[cpu_reg].w[5] + src[5] + 1;
        tmp[6] = cpu_state.XMM[cpu_reg].w[6] + src[6] + 1;
        tmp[7] = cpu_state.XMM[cpu_reg].w[7] + src[7] + 1;

        cpu_state.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
    }

    return 0;
}

static int
opPAVGW_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    uint32_t tmp[8];
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        tmp[0] = cpu_state.XMM[cpu_reg].w[0] + cpu_state.XMM[cpu_rm].w[0] + 1;
        tmp[1] = cpu_state.XMM[cpu_reg].w[1] + cpu_state.XMM[cpu_rm].w[1] + 1;
        tmp[2] = cpu_state.XMM[cpu_reg].w[2] + cpu_state.XMM[cpu_rm].w[2] + 1;
        tmp[3] = cpu_state.XMM[cpu_reg].w[3] + cpu_state.XMM[cpu_rm].w[3] + 1;
        tmp[4] = cpu_state.XMM[cpu_reg].w[4] + cpu_state.XMM[cpu_rm].w[4] + 1;
        tmp[5] = cpu_state.XMM[cpu_reg].w[5] + cpu_state.XMM[cpu_rm].w[5] + 1;
        tmp[6] = cpu_state.XMM[cpu_reg].w[6] + cpu_state.XMM[cpu_rm].w[6] + 1;
        tmp[7] = cpu_state.XMM[cpu_reg].w[7] + cpu_state.XMM[cpu_rm].w[7] + 1;

        cpu_state.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
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

        tmp[0] = cpu_state.XMM[cpu_reg].w[0] + src[0] + 1;
        tmp[1] = cpu_state.XMM[cpu_reg].w[1] + src[1] + 1;
        tmp[2] = cpu_state.XMM[cpu_reg].w[2] + src[2] + 1;
        tmp[3] = cpu_state.XMM[cpu_reg].w[3] + src[3] + 1;
        tmp[4] = cpu_state.XMM[cpu_reg].w[4] + src[4] + 1;
        tmp[5] = cpu_state.XMM[cpu_reg].w[5] + src[5] + 1;
        tmp[6] = cpu_state.XMM[cpu_reg].w[6] + src[6] + 1;
        tmp[7] = cpu_state.XMM[cpu_reg].w[7] + src[7] + 1;

        cpu_state.XMM[cpu_reg].w[0] = (uint16_t) (tmp[0] >> 1);
        cpu_state.XMM[cpu_reg].w[1] = (uint16_t) (tmp[1] >> 1);
        cpu_state.XMM[cpu_reg].w[2] = (uint16_t) (tmp[2] >> 1);
        cpu_state.XMM[cpu_reg].w[3] = (uint16_t) (tmp[3] >> 1);
        cpu_state.XMM[cpu_reg].w[4] = (uint16_t) (tmp[4] >> 1);
        cpu_state.XMM[cpu_reg].w[5] = (uint16_t) (tmp[5] >> 1);
        cpu_state.XMM[cpu_reg].w[6] = (uint16_t) (tmp[6] >> 1);
        cpu_state.XMM[cpu_reg].w[7] = (uint16_t) (tmp[7] >> 1);
    }

    return 0;
}

static int
opPAVGW_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPAVGW_xmm_xmm_a16(fetchdat);
    else
        return opPAVGW_mm_mm_a16(fetchdat);
}

static int
opPAVGW_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state.XMM[cpu_reg].w[0] * (uint32_t) cpu_state.XMM[cpu_rm].w[0]) >> 16;
        cpu_state.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state.XMM[cpu_reg].w[1] * (uint32_t) cpu_state.XMM[cpu_rm].w[1]) >> 16;
        cpu_state.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state.XMM[cpu_reg].w[2] * (uint32_t) cpu_state.XMM[cpu_rm].w[2]) >> 16;
        cpu_state.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state.XMM[cpu_reg].w[3] * (uint32_t) cpu_state.XMM[cpu_rm].w[3]) >> 16;
        cpu_state.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state.XMM[cpu_reg].w[4] * (uint32_t) cpu_state.XMM[cpu_rm].w[4]) >> 16;
        cpu_state.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state.XMM[cpu_reg].w[5] * (uint32_t) cpu_state.XMM[cpu_rm].w[5]) >> 16;
        cpu_state.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state.XMM[cpu_reg].w[6] * (uint32_t) cpu_state.XMM[cpu_rm].w[6]) >> 16;
        cpu_state.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state.XMM[cpu_reg].w[7] * (uint32_t) cpu_state.XMM[cpu_rm].w[7]) >> 16;
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
        cpu_state.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state.XMM[cpu_reg].w[0] * src[0]) >> 16;
        cpu_state.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state.XMM[cpu_reg].w[1] * src[1]) >> 16;
        cpu_state.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state.XMM[cpu_reg].w[2] * src[2]) >> 16;
        cpu_state.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state.XMM[cpu_reg].w[3] * src[3]) >> 16;
        cpu_state.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state.XMM[cpu_reg].w[4] * src[4]) >> 16;
        cpu_state.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state.XMM[cpu_reg].w[5] * src[5]) >> 16;
        cpu_state.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state.XMM[cpu_reg].w[6] * src[6]) >> 16;
        cpu_state.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state.XMM[cpu_reg].w[7] * src[7]) >> 16;
    }

    return 0;
}

static int
opPMULHUW_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state.XMM[cpu_reg].w[0] * (uint32_t) cpu_state.XMM[cpu_rm].w[0]) >> 16;
        cpu_state.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state.XMM[cpu_reg].w[1] * (uint32_t) cpu_state.XMM[cpu_rm].w[1]) >> 16;
        cpu_state.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state.XMM[cpu_reg].w[2] * (uint32_t) cpu_state.XMM[cpu_rm].w[2]) >> 16;
        cpu_state.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state.XMM[cpu_reg].w[3] * (uint32_t) cpu_state.XMM[cpu_rm].w[3]) >> 16;
        cpu_state.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state.XMM[cpu_reg].w[4] * (uint32_t) cpu_state.XMM[cpu_rm].w[4]) >> 16;
        cpu_state.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state.XMM[cpu_reg].w[5] * (uint32_t) cpu_state.XMM[cpu_rm].w[5]) >> 16;
        cpu_state.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state.XMM[cpu_reg].w[6] * (uint32_t) cpu_state.XMM[cpu_rm].w[6]) >> 16;
        cpu_state.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state.XMM[cpu_reg].w[7] * (uint32_t) cpu_state.XMM[cpu_rm].w[7]) >> 16;
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
        cpu_state.XMM[cpu_reg].w[0] = ((uint32_t) cpu_state.XMM[cpu_reg].w[0] * src[0]) >> 16;
        cpu_state.XMM[cpu_reg].w[1] = ((uint32_t) cpu_state.XMM[cpu_reg].w[1] * src[1]) >> 16;
        cpu_state.XMM[cpu_reg].w[2] = ((uint32_t) cpu_state.XMM[cpu_reg].w[2] * src[2]) >> 16;
        cpu_state.XMM[cpu_reg].w[3] = ((uint32_t) cpu_state.XMM[cpu_reg].w[3] * src[3]) >> 16;
        cpu_state.XMM[cpu_reg].w[4] = ((uint32_t) cpu_state.XMM[cpu_reg].w[4] * src[4]) >> 16;
        cpu_state.XMM[cpu_reg].w[5] = ((uint32_t) cpu_state.XMM[cpu_reg].w[5] * src[5]) >> 16;
        cpu_state.XMM[cpu_reg].w[6] = ((uint32_t) cpu_state.XMM[cpu_reg].w[6] * src[6]) >> 16;
        cpu_state.XMM[cpu_reg].w[7] = ((uint32_t) cpu_state.XMM[cpu_reg].w[7] * src[7]) >> 16;
    }

    return 0;
}

static int
opPMULHUW_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPMULHUW_xmm_xmm_a16(fetchdat);
    else
        return opPMULHUW_mm_mm_a16(fetchdat);
}

static int
opPMULHUW_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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

    dst->sw[0] = (dst->sw[0] < src.sw[0]) ? dst->sw[0] : src.sw[0];
    dst->sw[1] = (dst->sw[1] < src.sw[1]) ? dst->sw[1] : src.sw[1];
    dst->sw[2] = (dst->sw[2] < src.sw[2]) ? dst->sw[2] : src.sw[2];
    dst->sw[3] = (dst->sw[3] < src.sw[3]) ? dst->sw[3] : src.sw[3];

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

    dst->sw[0] = (dst->sw[0] < src.sw[0]) ? dst->sw[0] : src.sw[0];
    dst->sw[1] = (dst->sw[1] < src.sw[1]) ? dst->sw[1] : src.sw[1];
    dst->sw[2] = (dst->sw[2] < src.sw[2]) ? dst->sw[2] : src.sw[2];
    dst->sw[3] = (dst->sw[3] < src.sw[3]) ? dst->sw[3] : src.sw[3];

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMINSW_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] < cpu_state.XMM[cpu_rm].sw[0]) ? cpu_state.XMM[cpu_reg].sw[0] : cpu_state.XMM[cpu_rm].sw[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] < cpu_state.XMM[cpu_rm].sw[1]) ? cpu_state.XMM[cpu_reg].sw[1] : cpu_state.XMM[cpu_rm].sw[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] < cpu_state.XMM[cpu_rm].sw[2]) ? cpu_state.XMM[cpu_reg].sw[2] : cpu_state.XMM[cpu_rm].sw[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] < cpu_state.XMM[cpu_rm].sw[3]) ? cpu_state.XMM[cpu_reg].sw[3] : cpu_state.XMM[cpu_rm].sw[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] < cpu_state.XMM[cpu_rm].sw[4]) ? cpu_state.XMM[cpu_reg].sw[4] : cpu_state.XMM[cpu_rm].sw[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] < cpu_state.XMM[cpu_rm].sw[5]) ? cpu_state.XMM[cpu_reg].sw[5] : cpu_state.XMM[cpu_rm].sw[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] < cpu_state.XMM[cpu_rm].sw[6]) ? cpu_state.XMM[cpu_reg].sw[6] : cpu_state.XMM[cpu_rm].sw[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] < cpu_state.XMM[cpu_rm].sw[7]) ? cpu_state.XMM[cpu_reg].sw[7] : cpu_state.XMM[cpu_rm].sw[7];
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
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] < src[0]) ? cpu_state.XMM[cpu_reg].sw[0] : src[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] < src[1]) ? cpu_state.XMM[cpu_reg].sw[1] : src[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] < src[2]) ? cpu_state.XMM[cpu_reg].sw[2] : src[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] < src[3]) ? cpu_state.XMM[cpu_reg].sw[3] : src[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] < src[4]) ? cpu_state.XMM[cpu_reg].sw[4] : src[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] < src[5]) ? cpu_state.XMM[cpu_reg].sw[5] : src[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] < src[6]) ? cpu_state.XMM[cpu_reg].sw[6] : src[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] < src[7]) ? cpu_state.XMM[cpu_reg].sw[7] : src[7];
    }

    return 0;
}

static int
opPMINSW_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] < cpu_state.XMM[cpu_rm].sw[0]) ? cpu_state.XMM[cpu_reg].sw[0] : cpu_state.XMM[cpu_rm].sw[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] < cpu_state.XMM[cpu_rm].sw[1]) ? cpu_state.XMM[cpu_reg].sw[1] : cpu_state.XMM[cpu_rm].sw[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] < cpu_state.XMM[cpu_rm].sw[2]) ? cpu_state.XMM[cpu_reg].sw[2] : cpu_state.XMM[cpu_rm].sw[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] < cpu_state.XMM[cpu_rm].sw[3]) ? cpu_state.XMM[cpu_reg].sw[3] : cpu_state.XMM[cpu_rm].sw[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] < cpu_state.XMM[cpu_rm].sw[4]) ? cpu_state.XMM[cpu_reg].sw[4] : cpu_state.XMM[cpu_rm].sw[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] < cpu_state.XMM[cpu_rm].sw[5]) ? cpu_state.XMM[cpu_reg].sw[5] : cpu_state.XMM[cpu_rm].sw[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] < cpu_state.XMM[cpu_rm].sw[6]) ? cpu_state.XMM[cpu_reg].sw[6] : cpu_state.XMM[cpu_rm].sw[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] < cpu_state.XMM[cpu_rm].sw[7]) ? cpu_state.XMM[cpu_reg].sw[7] : cpu_state.XMM[cpu_rm].sw[7];
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
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] < src[0]) ? cpu_state.XMM[cpu_reg].sw[0] : src[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] < src[1]) ? cpu_state.XMM[cpu_reg].sw[1] : src[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] < src[2]) ? cpu_state.XMM[cpu_reg].sw[2] : src[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] < src[3]) ? cpu_state.XMM[cpu_reg].sw[3] : src[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] < src[4]) ? cpu_state.XMM[cpu_reg].sw[4] : src[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] < src[5]) ? cpu_state.XMM[cpu_reg].sw[5] : src[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] < src[6]) ? cpu_state.XMM[cpu_reg].sw[6] : src[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] < src[7]) ? cpu_state.XMM[cpu_reg].sw[7] : src[7];
    }

    return 0;
}

static int
opPMINSW_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPMINSW_xmm_xmm_a16(fetchdat);
    else
        return opPMINSW_mm_mm_a16(fetchdat);
}

static int
opPMINSW_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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

    dst->sw[0] = (dst->sw[0] > src.sw[0]) ? dst->sw[0] : src.sw[0];
    dst->sw[1] = (dst->sw[1] > src.sw[1]) ? dst->sw[1] : src.sw[1];
    dst->sw[2] = (dst->sw[2] > src.sw[2]) ? dst->sw[2] : src.sw[2];
    dst->sw[3] = (dst->sw[3] > src.sw[3]) ? dst->sw[3] : src.sw[3];

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

    dst->sw[0] = (dst->sw[0] > src.sw[0]) ? dst->sw[0] : src.sw[0];
    dst->sw[1] = (dst->sw[1] > src.sw[1]) ? dst->sw[1] : src.sw[1];
    dst->sw[2] = (dst->sw[2] > src.sw[2]) ? dst->sw[2] : src.sw[2];
    dst->sw[3] = (dst->sw[3] > src.sw[3]) ? dst->sw[3] : src.sw[3];

    CLOCK_CYCLES(1);
    MMX_SETEXP(cpu_reg); 

    return 0;
}

static int
opPMAXSW_xmm_xmm_a16(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] > cpu_state.XMM[cpu_rm].sw[0]) ? cpu_state.XMM[cpu_reg].sw[0] : cpu_state.XMM[cpu_rm].sw[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] > cpu_state.XMM[cpu_rm].sw[1]) ? cpu_state.XMM[cpu_reg].sw[1] : cpu_state.XMM[cpu_rm].sw[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] > cpu_state.XMM[cpu_rm].sw[2]) ? cpu_state.XMM[cpu_reg].sw[2] : cpu_state.XMM[cpu_rm].sw[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] > cpu_state.XMM[cpu_rm].sw[3]) ? cpu_state.XMM[cpu_reg].sw[3] : cpu_state.XMM[cpu_rm].sw[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] > cpu_state.XMM[cpu_rm].sw[4]) ? cpu_state.XMM[cpu_reg].sw[4] : cpu_state.XMM[cpu_rm].sw[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] > cpu_state.XMM[cpu_rm].sw[5]) ? cpu_state.XMM[cpu_reg].sw[5] : cpu_state.XMM[cpu_rm].sw[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] > cpu_state.XMM[cpu_rm].sw[6]) ? cpu_state.XMM[cpu_reg].sw[6] : cpu_state.XMM[cpu_rm].sw[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] > cpu_state.XMM[cpu_rm].sw[7]) ? cpu_state.XMM[cpu_reg].sw[7] : cpu_state.XMM[cpu_rm].sw[7];
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
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] > src[0]) ? cpu_state.XMM[cpu_reg].sw[0] : src[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] > src[1]) ? cpu_state.XMM[cpu_reg].sw[1] : src[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] > src[2]) ? cpu_state.XMM[cpu_reg].sw[2] : src[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] > src[3]) ? cpu_state.XMM[cpu_reg].sw[3] : src[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] > src[4]) ? cpu_state.XMM[cpu_reg].sw[4] : src[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] > src[5]) ? cpu_state.XMM[cpu_reg].sw[5] : src[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] > src[6]) ? cpu_state.XMM[cpu_reg].sw[6] : src[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] > src[7]) ? cpu_state.XMM[cpu_reg].sw[7] : src[7];
    }

    return 0;
}

static int
opPMAXSW_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] > cpu_state.XMM[cpu_rm].sw[0]) ? cpu_state.XMM[cpu_reg].sw[0] : cpu_state.XMM[cpu_rm].sw[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] > cpu_state.XMM[cpu_rm].sw[1]) ? cpu_state.XMM[cpu_reg].sw[1] : cpu_state.XMM[cpu_rm].sw[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] > cpu_state.XMM[cpu_rm].sw[2]) ? cpu_state.XMM[cpu_reg].sw[2] : cpu_state.XMM[cpu_rm].sw[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] > cpu_state.XMM[cpu_rm].sw[3]) ? cpu_state.XMM[cpu_reg].sw[3] : cpu_state.XMM[cpu_rm].sw[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] > cpu_state.XMM[cpu_rm].sw[4]) ? cpu_state.XMM[cpu_reg].sw[4] : cpu_state.XMM[cpu_rm].sw[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] > cpu_state.XMM[cpu_rm].sw[5]) ? cpu_state.XMM[cpu_reg].sw[5] : cpu_state.XMM[cpu_rm].sw[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] > cpu_state.XMM[cpu_rm].sw[6]) ? cpu_state.XMM[cpu_reg].sw[6] : cpu_state.XMM[cpu_rm].sw[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] > cpu_state.XMM[cpu_rm].sw[7]) ? cpu_state.XMM[cpu_reg].sw[7] : cpu_state.XMM[cpu_rm].sw[7];
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
        cpu_state.XMM[cpu_reg].sw[0] = (cpu_state.XMM[cpu_reg].sw[0] > src[0]) ? cpu_state.XMM[cpu_reg].sw[0] : src[0];
        cpu_state.XMM[cpu_reg].sw[1] = (cpu_state.XMM[cpu_reg].sw[1] > src[1]) ? cpu_state.XMM[cpu_reg].sw[1] : src[1];
        cpu_state.XMM[cpu_reg].sw[2] = (cpu_state.XMM[cpu_reg].sw[2] > src[2]) ? cpu_state.XMM[cpu_reg].sw[2] : src[2];
        cpu_state.XMM[cpu_reg].sw[3] = (cpu_state.XMM[cpu_reg].sw[3] > src[3]) ? cpu_state.XMM[cpu_reg].sw[3] : src[3];
        cpu_state.XMM[cpu_reg].sw[4] = (cpu_state.XMM[cpu_reg].sw[4] > src[4]) ? cpu_state.XMM[cpu_reg].sw[4] : src[4];
        cpu_state.XMM[cpu_reg].sw[5] = (cpu_state.XMM[cpu_reg].sw[5] > src[5]) ? cpu_state.XMM[cpu_reg].sw[5] : src[5];
        cpu_state.XMM[cpu_reg].sw[6] = (cpu_state.XMM[cpu_reg].sw[6] > src[6]) ? cpu_state.XMM[cpu_reg].sw[6] : src[6];
        cpu_state.XMM[cpu_reg].sw[7] = (cpu_state.XMM[cpu_reg].sw[7] > src[7]) ? cpu_state.XMM[cpu_reg].sw[7] : src[7];
    }

    return 0;
}

static int
opPMAXSW_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPMAXSW_xmm_xmm_a16(fetchdat);
    else
        return opPMAXSW_mm_mm_a16(fetchdat);
}

static int
opPMAXSW_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
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

    temp.b[0] = abs(dst->b[0] - src.b[0]);
    temp.b[1] = abs(dst->b[1] - src.b[1]);
    temp.b[2] = abs(dst->b[2] - src.b[2]);
    temp.b[3] = abs(dst->b[3] - src.b[3]);
    temp.b[4] = abs(dst->b[4] - src.b[4]);
    temp.b[5] = abs(dst->b[5] - src.b[5]);
    temp.b[6] = abs(dst->b[6] - src.b[6]);
    temp.b[7] = abs(dst->b[7] - src.b[7]);

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

    temp.b[0] = abs(dst->b[0] - src.b[0]);
    temp.b[1] = abs(dst->b[1] - src.b[1]);
    temp.b[2] = abs(dst->b[2] - src.b[2]);
    temp.b[3] = abs(dst->b[3] - src.b[3]);
    temp.b[4] = abs(dst->b[4] - src.b[4]);
    temp.b[5] = abs(dst->b[5] - src.b[5]);
    temp.b[6] = abs(dst->b[6] - src.b[6]);
    temp.b[7] = abs(dst->b[7] - src.b[7]);

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
    SSE_ENTER();
    fetch_ea_16(fetchdat);
    if (cpu_mod == 3) {
        uint8_t temp[16];
        temp[0]         = abs(cpu_state.XMM[cpu_reg].b[0] - cpu_state.XMM[cpu_rm].b[0]);
        temp[1]         = abs(cpu_state.XMM[cpu_reg].b[1] - cpu_state.XMM[cpu_rm].b[1]);
        temp[2]         = abs(cpu_state.XMM[cpu_reg].b[2] - cpu_state.XMM[cpu_rm].b[2]);
        temp[3]         = abs(cpu_state.XMM[cpu_reg].b[3] - cpu_state.XMM[cpu_rm].b[3]);
        temp[4]         = abs(cpu_state.XMM[cpu_reg].b[4] - cpu_state.XMM[cpu_rm].b[4]);
        temp[5]         = abs(cpu_state.XMM[cpu_reg].b[5] - cpu_state.XMM[cpu_rm].b[5]);
        temp[6]         = abs(cpu_state.XMM[cpu_reg].b[6] - cpu_state.XMM[cpu_rm].b[6]);
        temp[7]         = abs(cpu_state.XMM[cpu_reg].b[7] - cpu_state.XMM[cpu_rm].b[7]);
        temp[8]         = abs(cpu_state.XMM[cpu_reg].b[8] - cpu_state.XMM[cpu_rm].b[8]);
        temp[9]         = abs(cpu_state.XMM[cpu_reg].b[9] - cpu_state.XMM[cpu_rm].b[9]);
        temp[10]        = abs(cpu_state.XMM[cpu_reg].b[10] - cpu_state.XMM[cpu_rm].b[10]);
        temp[11]        = abs(cpu_state.XMM[cpu_reg].b[11] - cpu_state.XMM[cpu_rm].b[11]);
        temp[12]        = abs(cpu_state.XMM[cpu_reg].b[12] - cpu_state.XMM[cpu_rm].b[12]);
        temp[13]        = abs(cpu_state.XMM[cpu_reg].b[13] - cpu_state.XMM[cpu_rm].b[13]);
        temp[14]        = abs(cpu_state.XMM[cpu_reg].b[14] - cpu_state.XMM[cpu_rm].b[14]);
        temp[15]        = abs(cpu_state.XMM[cpu_reg].b[15] - cpu_state.XMM[cpu_rm].b[15]);
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state.XMM[cpu_reg].w[0] = result;
        cpu_state.XMM[cpu_reg].w[1] = 0;
        cpu_state.XMM[cpu_reg].w[2] = 0;
        cpu_state.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state.XMM[cpu_reg].w[4] = result;
        cpu_state.XMM[cpu_reg].w[5] = 0;
        cpu_state.XMM[cpu_reg].w[6] = 0;
        cpu_state.XMM[cpu_reg].w[7] = 0;
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
        temp[0]         = abs(cpu_state.XMM[cpu_reg].b[0] - src[0]);
        temp[1]         = abs(cpu_state.XMM[cpu_reg].b[1] - src[1]);
        temp[2]         = abs(cpu_state.XMM[cpu_reg].b[2] - src[2]);
        temp[3]         = abs(cpu_state.XMM[cpu_reg].b[3] - src[3]);
        temp[4]         = abs(cpu_state.XMM[cpu_reg].b[4] - src[4]);
        temp[5]         = abs(cpu_state.XMM[cpu_reg].b[5] - src[5]);
        temp[6]         = abs(cpu_state.XMM[cpu_reg].b[6] - src[6]);
        temp[7]         = abs(cpu_state.XMM[cpu_reg].b[7] - src[7]);
        temp[8]         = abs(cpu_state.XMM[cpu_reg].b[8] - src[8]);
        temp[9]         = abs(cpu_state.XMM[cpu_reg].b[9] - src[9]);
        temp[10]        = abs(cpu_state.XMM[cpu_reg].b[10] - src[10]);
        temp[11]        = abs(cpu_state.XMM[cpu_reg].b[11] - src[11]);
        temp[12]        = abs(cpu_state.XMM[cpu_reg].b[12] - src[12]);
        temp[13]        = abs(cpu_state.XMM[cpu_reg].b[13] - src[13]);
        temp[14]        = abs(cpu_state.XMM[cpu_reg].b[14] - src[14]);
        temp[15]        = abs(cpu_state.XMM[cpu_reg].b[15] - src[15]);
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state.XMM[cpu_reg].w[0] = result;
        cpu_state.XMM[cpu_reg].w[1] = 0;
        cpu_state.XMM[cpu_reg].w[2] = 0;
        cpu_state.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state.XMM[cpu_reg].w[4] = result;
        cpu_state.XMM[cpu_reg].w[5] = 0;
        cpu_state.XMM[cpu_reg].w[6] = 0;
        cpu_state.XMM[cpu_reg].w[7] = 0;
    }

    return 0;
}

static int
opPSADBW_xmm_xmm_a32(uint32_t fetchdat)
{
    SSE_ENTER();
    fetch_ea_32(fetchdat);
    if (cpu_mod == 3) {
        uint8_t temp[16];
        temp[0]         = abs(cpu_state.XMM[cpu_reg].b[0] - cpu_state.XMM[cpu_rm].b[0]);
        temp[1]         = abs(cpu_state.XMM[cpu_reg].b[1] - cpu_state.XMM[cpu_rm].b[1]);
        temp[2]         = abs(cpu_state.XMM[cpu_reg].b[2] - cpu_state.XMM[cpu_rm].b[2]);
        temp[3]         = abs(cpu_state.XMM[cpu_reg].b[3] - cpu_state.XMM[cpu_rm].b[3]);
        temp[4]         = abs(cpu_state.XMM[cpu_reg].b[4] - cpu_state.XMM[cpu_rm].b[4]);
        temp[5]         = abs(cpu_state.XMM[cpu_reg].b[5] - cpu_state.XMM[cpu_rm].b[5]);
        temp[6]         = abs(cpu_state.XMM[cpu_reg].b[6] - cpu_state.XMM[cpu_rm].b[6]);
        temp[7]         = abs(cpu_state.XMM[cpu_reg].b[7] - cpu_state.XMM[cpu_rm].b[7]);
        temp[8]         = abs(cpu_state.XMM[cpu_reg].b[8] - cpu_state.XMM[cpu_rm].b[8]);
        temp[9]         = abs(cpu_state.XMM[cpu_reg].b[9] - cpu_state.XMM[cpu_rm].b[9]);
        temp[10]        = abs(cpu_state.XMM[cpu_reg].b[10] - cpu_state.XMM[cpu_rm].b[10]);
        temp[11]        = abs(cpu_state.XMM[cpu_reg].b[11] - cpu_state.XMM[cpu_rm].b[11]);
        temp[12]        = abs(cpu_state.XMM[cpu_reg].b[12] - cpu_state.XMM[cpu_rm].b[12]);
        temp[13]        = abs(cpu_state.XMM[cpu_reg].b[13] - cpu_state.XMM[cpu_rm].b[13]);
        temp[14]        = abs(cpu_state.XMM[cpu_reg].b[14] - cpu_state.XMM[cpu_rm].b[14]);
        temp[15]        = abs(cpu_state.XMM[cpu_reg].b[15] - cpu_state.XMM[cpu_rm].b[15]);
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state.XMM[cpu_reg].w[0] = result;
        cpu_state.XMM[cpu_reg].w[1] = 0;
        cpu_state.XMM[cpu_reg].w[2] = 0;
        cpu_state.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state.XMM[cpu_reg].w[4] = result;
        cpu_state.XMM[cpu_reg].w[5] = 0;
        cpu_state.XMM[cpu_reg].w[6] = 0;
        cpu_state.XMM[cpu_reg].w[7] = 0;
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
        temp[0]         = abs(cpu_state.XMM[cpu_reg].b[0] - src[0]);
        temp[1]         = abs(cpu_state.XMM[cpu_reg].b[1] - src[1]);
        temp[2]         = abs(cpu_state.XMM[cpu_reg].b[2] - src[2]);
        temp[3]         = abs(cpu_state.XMM[cpu_reg].b[3] - src[3]);
        temp[4]         = abs(cpu_state.XMM[cpu_reg].b[4] - src[4]);
        temp[5]         = abs(cpu_state.XMM[cpu_reg].b[5] - src[5]);
        temp[6]         = abs(cpu_state.XMM[cpu_reg].b[6] - src[6]);
        temp[7]         = abs(cpu_state.XMM[cpu_reg].b[7] - src[7]);
        temp[8]         = abs(cpu_state.XMM[cpu_reg].b[8] - src[8]);
        temp[9]         = abs(cpu_state.XMM[cpu_reg].b[9] - src[9]);
        temp[10]        = abs(cpu_state.XMM[cpu_reg].b[10] - src[10]);
        temp[11]        = abs(cpu_state.XMM[cpu_reg].b[11] - src[11]);
        temp[12]        = abs(cpu_state.XMM[cpu_reg].b[12] - src[12]);
        temp[13]        = abs(cpu_state.XMM[cpu_reg].b[13] - src[13]);
        temp[14]        = abs(cpu_state.XMM[cpu_reg].b[14] - src[14]);
        temp[15]        = abs(cpu_state.XMM[cpu_reg].b[15] - src[15]);
        uint16_t result = 0;
        for (int i = 0; i < 8; i++) {
            result += temp[i];
        }
        cpu_state.XMM[cpu_reg].w[0] = result;
        cpu_state.XMM[cpu_reg].w[1] = 0;
        cpu_state.XMM[cpu_reg].w[2] = 0;
        cpu_state.XMM[cpu_reg].w[3] = 0;

        result = 0;
        for (int i = 8; i < 16; i++) {
            result += temp[i];
        }

        cpu_state.XMM[cpu_reg].w[4] = result;
        cpu_state.XMM[cpu_reg].w[5] = 0;
        cpu_state.XMM[cpu_reg].w[6] = 0;
        cpu_state.XMM[cpu_reg].w[7] = 0;
    }

    return 0;
}

static int
opPSADBW_a16(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPSADBW_xmm_xmm_a16(fetchdat);
    else
        return opPSADBW_mm_mm_a16(fetchdat);
}

static int
opPSADBW_a32(uint32_t fetchdat)
{
    if (cpu_state.sse_xmm)
        return opPSADBW_xmm_xmm_a32(fetchdat);
    else
        return opPSADBW_mm_mm_a32(fetchdat);
}
