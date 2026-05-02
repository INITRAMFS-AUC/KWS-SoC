/* accel_test.c — Standalone unit test for the Conv1D hardware accelerator.
 *
 * Runs three test cases without I2S audio collection:
 *   TC1: C_in=1, C_out=2, K_w=3, stride=1, W_in=5  — tiny, verify basic MAC+bias+shift
 *   TC2: C_in=1, C_out=1, K_w=65, stride=1, W_in=68 — front-end kernel size
 *   TC3: C_in=4, C_out=4, K_w=3, stride=1, W_in=6   — multi-channel body layer
 *
 * Weights are placed in .rodata (→ flash/XIP) to exercise the XIP cache path.
 * Input, bias, shift, and output buffers are in SRAM (.bss / stack).
 *
 * Expected output is computed with a software reference on the same data.
 * UART output: "PASS TC1\n" / "FAIL TC1 out[i]=X exp=Y\n" etc.
 */

#include <stdint.h>
#include <string.h>
#include "uart_utils.h"

/* ---- Accelerator register map ---- */
#define ACCEL_BASE       0x4000C000UL
#define ACCEL_REG(off)   (*(volatile uint32_t *)(ACCEL_BASE + (off)))
#define ACCEL_CTRL       ACCEL_REG(0x00)
#define ACCEL_SRC_ADDR   ACCEL_REG(0x04)
#define ACCEL_WT_ADDR    ACCEL_REG(0x08)
#define ACCEL_DST_ADDR   ACCEL_REG(0x0C)
#define ACCEL_BS_ADDR    ACCEL_REG(0x10)
#define ACCEL_CFG0       ACCEL_REG(0x14)
#define ACCEL_CFG1       ACCEL_REG(0x18)
#define ACCEL_SHIFT_ADDR ACCEL_REG(0x20)
#define ACCEL_CTRL_START (1u << 0)
#define ACCEL_CTRL_DONE  (1u << 9)

static void accel_run(
    const int8_t  *src, const int8_t *wt, int8_t *dst,
    const int32_t *bias32, const uint8_t *shift_arr,
    uint8_t c_in, uint8_t c_out, uint8_t k_w, uint8_t stride, uint16_t w_in)
{
    ACCEL_SRC_ADDR   = (uint32_t)(uintptr_t)src;
    ACCEL_WT_ADDR    = (uint32_t)(uintptr_t)wt;
    ACCEL_DST_ADDR   = (uint32_t)(uintptr_t)dst;
    ACCEL_BS_ADDR    = (uint32_t)(uintptr_t)bias32;
    ACCEL_SHIFT_ADDR = (uint32_t)(uintptr_t)shift_arr;
    ACCEL_CFG0 = ((uint32_t)stride << 24) | ((uint32_t)k_w << 16)
               | ((uint32_t)c_out  <<  8) | (uint32_t)c_in;
    ACCEL_CFG1 = (uint32_t)w_in;
    ACCEL_CTRL = ACCEL_CTRL_START;
    while (!(ACCEL_CTRL & ACCEL_CTRL_DONE));
}

/* Software reference: valid-padding conv, int8 input/weight, int32 accumulate,
 * then add bias32 and right-shift, clip to int8. */
static void sw_conv1d_ref(
    const int8_t  *src, const int8_t *wt, int8_t *dst,
    const int32_t *bias32, const uint8_t *shift_arr,
    uint8_t c_in, uint8_t c_out, uint8_t k_w, uint8_t stride, uint16_t w_in)
{
    uint16_t w_out = (uint16_t)((w_in - k_w) / stride + 1);
    for (uint16_t w = 0; w < w_out; w++) {
        for (uint8_t co = 0; co < c_out; co++) {
            int32_t acc = bias32[co];
            for (uint8_t k = 0; k < k_w; k++) {
                for (uint8_t ci = 0; ci < c_in; ci++) {
                    int32_t s = src[(w * stride + k) * c_in + ci];
                    int32_t weight = wt[co * k_w * c_in + k * c_in + ci];
                    acc += s * weight;
                }
            }
            int32_t shifted = acc >> shift_arr[co];
            int8_t clipped = (shifted > 127) ? 127 : (shifted < -128) ? -128 : (int8_t)shifted;
            dst[w * c_out + co] = clipped;
        }
    }
}

static int check(const char *name, const int8_t *got, const int8_t *exp, int n)
{
    for (int i = 0; i < n; i++) {
        if (got[i] != exp[i]) {
            printf("FAIL %s out[%d]=%d exp=%d\n", name, i, (int)got[i], (int)exp[i]);
            return 0;
        }
    }
    printf("PASS %s\n", name);
    return 1;
}

/* ================================================================
 * Test case 1: tiny  C_in=1, C_out=2, K_w=3, stride=1, W_in=5
 * ================================================================ */

/* Weights in flash (.rodata) */
static const int8_t tc1_wt[2][3] = {
    { 1,  2,  3},   /* filter 0 */
    {-1, -2, -3},   /* filter 1 */
};

static void test_tc1(void)
{
    static int8_t  src[5]  = {1, 2, 3, 4, 5};
    static int32_t bias[2] = {0, 0};
    static uint8_t shft[2] = {0, 0};
    static int8_t  dst[3*2];
    static int8_t  exp[3*2];

    sw_conv1d_ref(src, (const int8_t*)tc1_wt, exp, bias, shft, 1, 2, 3, 1, 5);
    memset(dst, 0, sizeof(dst));
    accel_run(src, (const int8_t*)tc1_wt, dst, bias, shft, 1, 2, 3, 1, 5);
    check("TC1", dst, exp, 6);
}

/* ================================================================
 * Test case 2: K_w=65 front-end kernel, C_in=1, C_out=1, W_in=68
 * ================================================================ */

static const int8_t tc2_wt[65] = {
     1,-1, 2,-2, 3,-3, 4,-4, 5,-5, 6,-6, 7,-7, 8,-8,
     9,-9,10,-10,11,-11,12,-12,13,-13, 1,-1, 2,-2, 3,-3,
     4,-4, 5,-5, 6,-6, 7,-7, 8,-8, 9,-9,10,-10,11,-11,
    12,-12,13,-13, 1,-1, 2,-2, 3,-3, 4,-4, 5,-5, 6,-6,
     7
};

static void test_tc2(void)
{
    static int8_t  src[68];
    static int32_t bias[1] = {0};
    static uint8_t shft[1] = {7};   /* shift 7 so 65*127^2 doesn't overflow */
    static int8_t  dst[4];          /* W_out = 68-65+1 = 4 */
    static int8_t  exp[4];

    /* Simple alternating input */
    for (int i = 0; i < 68; i++) src[i] = (int8_t)(i & 1 ? 1 : -1);

    sw_conv1d_ref(src, tc2_wt, exp, bias, shft, 1, 1, 65, 1, 68);
    memset(dst, 0, sizeof(dst));
    accel_run(src, tc2_wt, dst, bias, shft, 1, 1, 65, 1, 68);
    check("TC2", dst, exp, 4);
}

/* ================================================================
 * Test case 3: multi-channel  C_in=4, C_out=4, K_w=3, stride=1, W_in=6
 * ================================================================ */

/* 4 filters x 3 positions x 4 channels = 48 bytes */
static const int8_t tc3_wt[4][3][4] = {
    {{ 1, 0, 0, 0}, { 0, 1, 0, 0}, { 0, 0, 1, 0}},  /* filter 0: identity-ish */
    {{ 0, 1, 0, 0}, { 0, 0, 1, 0}, { 0, 0, 0, 1}},  /* filter 1 */
    {{ 1, 1, 0, 0}, {-1,-1, 0, 0}, { 1, 1, 0, 0}},  /* filter 2 */
    {{ 0, 0, 1, 1}, { 0, 0,-1,-1}, { 0, 0, 1, 1}},  /* filter 3 */
};

static void test_tc3(void)
{
    /* W_in=6, C_in=4: 24 bytes — sequential values 1..24 */
    static int8_t src[6*4];
    static int32_t bias[4] = {10, -10, 5, -5};
    static uint8_t shft[4] = {1, 1, 1, 1};
    static int8_t dst[4*4];  /* W_out=4 */
    static int8_t exp[4*4];

    for (int i = 0; i < 24; i++) src[i] = (int8_t)(i + 1);

    sw_conv1d_ref(src, (const int8_t*)tc3_wt, exp, bias, shft, 4, 4, 3, 1, 6);
    memset(dst, 0, sizeof(dst));
    accel_run(src, (const int8_t*)tc3_wt, dst, bias, shft, 4, 4, 3, 1, 6);
    check("TC3", dst, exp, 16);
}

int main(void)
{
    uart_init();
    printf("ACCEL_TEST start\n");
    test_tc1();
    test_tc2();
    test_tc3();
    printf("ACCEL_TEST done\n");
    return 0;
}
