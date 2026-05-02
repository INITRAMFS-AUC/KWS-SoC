/*
 * conv1d_accel_tiny_golden_fw.c
 *
 * Firmware-level tiny Conv1D golden test using the APB scratchpad loader.
 *
 * Uses the same tensors as tb_kws_soc_conv1d_spad_loader.v so the expected
 * outputs are the verified RTL golden values. The test preloads input,
 * weights, and bias through SPAD_ADDR/SPAD_WDATA, runs the accelerator, and
 * reads outputs through SPAD_RDATA — no AHB master, no DMA, no NNoM.
 *
 * Requires the SoC to be built with -DCONV1D_USE_SIM_SCRATCHPAD=1.
 *
 * Expected UART output on success:
 *   CONV1D_ID:PASS
 *   CONV1D_TINY_FW:PASS
 *
 * On failure:
 *   CONV1D_ID:FAIL got=<got> exp=12345678
 *   -- or --
 *   CONV1D_TINY_FW:FAIL t=<t> oc=<oc> got=<got> exp=<exp>
 *   CONV1D_TINY_FW:FAIL
 */

#include <stdint.h>
#include "../../common/uart_utils.h"
#include "conv1d_accel_regs.h"

/* ------------------------------------------------------------------ */
/* Scratchpad address map (matches RTL testbench)                      */
/* ------------------------------------------------------------------ */
#define FW_INPUT_BASE   0x00000000u
#define FW_WEIGHT_BASE  0x00000100u
#define FW_BIAS_BASE    0x00000200u
#define FW_OUTPUT_BASE  0x00000300u

/* ------------------------------------------------------------------ */
/* Layer dimensions                                                    */
/* ------------------------------------------------------------------ */
#define FW_INPUT_LEN    8u
#define FW_IN_CH        4u
#define FW_OUT_CH       2u
#define FW_OUT_LEN      6u   /* INPUT_LEN - K + 1 = 8 - 3 + 1 */

/* ------------------------------------------------------------------ */
/* Tensor data (identical to tb_kws_soc_conv1d_spad_loader Part 2)    */
/*                                                                     */
/* Packing: word = {ic3, ic2, ic1, ic0} (LSB = ic0)                   */
/* ------------------------------------------------------------------ */

/* 8 time steps x 4 int8 channels, packed one word per time step */
static const uint32_t input_words[FW_INPUT_LEN] = {
    0x04030201u, /* t0: ic0= 1, ic1= 2, ic2= 3, ic3= 4 */
    0xFF000102u, /* t1: ic0= 2, ic1= 1, ic2= 0, ic3=-1 */
    0x000201FFu, /* t2: ic0=-1, ic1= 1, ic2= 2, ic3= 0 */
    0x0201FE03u, /* t3: ic0= 3, ic1=-2, ic2= 1, ic3= 2 */
    0x02FD0100u, /* t4: ic0= 0, ic1= 1, ic2=-3, ic3= 2 */
    0xFE020001u, /* t5: ic0= 1, ic1= 0, ic2= 2, ic3=-2 */
    0x01FF0202u, /* t6: ic0= 2, ic1= 2, ic2=-1, ic3= 1 */
    0x010003FEu, /* t7: ic0=-2, ic1= 3, ic2= 0, ic3= 1 */
};

/* 2 output channels x 3 kernel taps x 4 int8 input channels, oc-major */
static const uint32_t weight_words[FW_OUT_CH * 3] = {
    0x02FF0001u, /* oc0, k0: ic0= 1, ic1= 0, ic2=-1, ic3= 2 */
    0x00010100u, /* oc0, k1: ic0= 0, ic1= 1, ic2= 1, ic3= 0 */
    0x010002FFu, /* oc0, k2: ic0=-1, ic1= 2, ic2= 0, ic3= 1 */
    0x0100FF02u, /* oc1, k0: ic0= 2, ic1=-1, ic2= 0, ic3= 1 */
    0x00FF0101u, /* oc1, k1: ic0= 1, ic1= 1, ic2=-1, ic3= 0 */
    0x0101FE00u, /* oc1, k2: ic0= 0, ic1=-2, ic2= 1, ic3= 1 */
};

/* 2 output channel biases, one 32-bit int per channel */
static const uint32_t bias_words[FW_OUT_CH] = {
    0x00000000u, /* oc0 bias = 0 */
    0x00000001u, /* oc1 bias = 1 */
};

/* Per-output-channel right-shift for requantisation */
static const uint8_t out_shifts[FW_OUT_CH] = { 0u, 1u };

/* ------------------------------------------------------------------ */
/* Expected signed int8 outputs [t][oc]                               */
/* ------------------------------------------------------------------ */
static const int8_t expected[FW_OUT_LEN][FW_OUT_CH] = {
    { 10,  4 }, /* t0 */
    { -2,  4 }, /* t1 */
    {  0, -3 }, /* t2 */
    {  1,  7 }, /* t3 */
    { 12, -2 }, /* t4 */
    {  5,  0 }, /* t5 */
};

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static void spad_load(uint32_t base, const uint32_t *words, unsigned n)
{
    unsigned i;
    conv1d_spad_set_addr(base);
    for (i = 0; i < n; i++)
        conv1d_spad_write_word(words[i]);
}

/* ------------------------------------------------------------------ */
/* Test entry point                                                    */
/* ------------------------------------------------------------------ */

int main(void)
{
    uint32_t id;
    uint32_t timeout;
    uint32_t status;
    int failures = 0;
    unsigned t;

    uart_init();

    /* --- ID check -------------------------------------------------- */
    id = conv1d_read_reg(CONV1D_ID);
    if (id != CONV1D_EXPECTED_ID) {
        printf("CONV1D_ID:FAIL got=%lx exp=%lx\r\n",
               (unsigned long)id, (unsigned long)CONV1D_EXPECTED_ID);
        return 1;
    }
    printf("CONV1D_ID:PASS\r\n");

    /* --- Load tensors into scratchpad via APB loader --------------- */
    conv1d_spad_reset();

    spad_load(FW_INPUT_BASE,  input_words,  FW_INPUT_LEN);
    spad_load(FW_WEIGHT_BASE, weight_words, FW_OUT_CH * 3u);
    spad_load(FW_BIAS_BASE,   bias_words,   FW_OUT_CH);

    /* --- Configure accelerator ------------------------------------- */
    conv1d_write_reg(CONV1D_INPUT_BASE,  FW_INPUT_BASE);
    conv1d_write_reg(CONV1D_WEIGHT_BASE, FW_WEIGHT_BASE);
    conv1d_write_reg(CONV1D_BIAS_BASE,   FW_BIAS_BASE);
    conv1d_write_reg(CONV1D_OUTPUT_BASE, FW_OUTPUT_BASE);
    conv1d_write_reg(CONV1D_INPUT_LEN,   FW_INPUT_LEN);
    conv1d_write_reg(CONV1D_IN_CH,       FW_IN_CH);
    conv1d_write_reg(CONV1D_OUT_CH,      FW_OUT_CH);
    /* Write global QUANT=0 first (triggers load_all, overridden below) */
    conv1d_write_reg(CONV1D_QUANT,       0u);

    /* Per-output-channel shifts */
    conv1d_load_output_shifts_u8(out_shifts, (int)FW_OUT_CH);

    /* --- Start and poll done --------------------------------------- */
    conv1d_start();

    status  = 0u;
    timeout = 0u;
    while (!(status & CONV1D_STATUS_DONE) && timeout < 100000u) {
        status = conv1d_read_reg(CONV1D_STATUS);
        timeout++;
    }

    if (!(status & CONV1D_STATUS_DONE)) {
        printf("CONV1D_TINY_FW:FAIL timeout waiting for done\r\n");
        return 1;
    }

    /* --- Read and verify outputs ----------------------------------- */
    /*
     * Output layout: out_ch=2 bytes per time step, contiguous.
     * Each 32-bit word covers two consecutive time steps:
     *   word[7:0]   = oc0 @ t(2n)
     *   word[15:8]  = oc1 @ t(2n)
     *   word[23:16] = oc0 @ t(2n+1)
     *   word[31:24] = oc1 @ t(2n+1)
     */
    conv1d_spad_set_addr(FW_OUTPUT_BASE);

    for (t = 0; t < FW_OUT_LEN; t += 2u) {
        uint32_t word = conv1d_spad_read_word();
        int8_t got0 = (int8_t)( word        & 0xFFu);
        int8_t got1 = (int8_t)((word >>  8) & 0xFFu);
        int8_t got2 = (int8_t)((word >> 16) & 0xFFu);
        int8_t got3 = (int8_t)((word >> 24) & 0xFFu);

        if (got0 != expected[t][0]) {
            printf("CONV1D_TINY_FW:FAIL t=%u oc=0 got=%d exp=%d\r\n",
                   t, (int)got0, (int)expected[t][0]);
            failures++;
        }
        if (got1 != expected[t][1]) {
            printf("CONV1D_TINY_FW:FAIL t=%u oc=1 got=%d exp=%d\r\n",
                   t, (int)got1, (int)expected[t][1]);
            failures++;
        }
        if (t + 1u < FW_OUT_LEN) {
            if (got2 != expected[t + 1u][0]) {
                printf("CONV1D_TINY_FW:FAIL t=%u oc=0 got=%d exp=%d\r\n",
                       t + 1u, (int)got2, (int)expected[t + 1u][0]);
                failures++;
            }
            if (got3 != expected[t + 1u][1]) {
                printf("CONV1D_TINY_FW:FAIL t=%u oc=1 got=%d exp=%d\r\n",
                       t + 1u, (int)got3, (int)expected[t + 1u][1]);
                failures++;
            }
        }
    }

    if (failures == 0)
        printf("CONV1D_TINY_FW:PASS\r\n");
    else
        printf("CONV1D_TINY_FW:FAIL (%d mismatch(es))\r\n", failures);

    return (failures == 0) ? 0 : 1;
}
