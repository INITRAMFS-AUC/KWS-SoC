/*
 * conv1d_accel_smoke_test.c
 *
 * Smoke-test template for the Conv1D accelerator on KWS-SoC. This file is
 * intended as a template that the firmware tree drops in next to
 * conv1d_accel_regs.h. It is not built standalone in this repo because it
 * depends on the SoC's UART (or equivalent) print primitive and on the
 * SRAM/AHB memory wrapper that backs the accelerator's data ports.
 *
 * Suggested usage:
 *   1. ID check (APB-only path)
 *   2. Write/read APB config registers
 *   3. Defer CTRL.start until the memory-side wrapper is real
 */

#include <stdint.h>
#include "conv1d_accel_regs.h"

/* The firmware should provide a print primitive. Replace as appropriate. */
extern void uart_init(void);
extern int  uart_printf(const char *fmt, ...);

#ifndef CONV1D_DATA_BASE
#define CONV1D_DATA_BASE  0x20000000u
#endif

#define SMOKE_INPUT_BASE   (CONV1D_DATA_BASE + 0x0000u)
#define SMOKE_WEIGHT_BASE  (CONV1D_DATA_BASE + 0x0200u)
#define SMOKE_BIAS_BASE    (CONV1D_DATA_BASE + 0x0800u)
#define SMOKE_OUTPUT_BASE  (CONV1D_DATA_BASE + 0x0C00u)

#define SMOKE_INPUT_LEN    8u
#define SMOKE_IN_CH        4u
#define SMOKE_OUT_CH       2u
#define SMOKE_OUT_SHIFT    0u
#define SMOKE_RELU_EN      0

static int check_reg(const char *name, uint32_t offset, uint32_t expected)
{
    uint32_t got = conv1d_read_reg(offset);

    if (got != expected) {
        uart_printf("CONV1D_APB: FAIL %s got 0x%x expected 0x%x\r\n",
                    name, got, expected);
        return -1;
    }

    return 0;
}

int conv1d_smoke_test(void)
{
    uint32_t id = conv1d_read_reg(CONV1D_ID);

    uart_printf("CONV1D_ID: 0x%x\r\n", id);
    if (id != CONV1D_EXPECTED_ID) {
        uart_printf("CONV1D_APB: FAIL expected 0x%x\r\n",
                    CONV1D_EXPECTED_ID);
        return -1;
    }

    conv1d_write_reg(CONV1D_INPUT_BASE, SMOKE_INPUT_BASE);
    conv1d_write_reg(CONV1D_WEIGHT_BASE, SMOKE_WEIGHT_BASE);
    conv1d_write_reg(CONV1D_BIAS_BASE, SMOKE_BIAS_BASE);
    conv1d_write_reg(CONV1D_OUTPUT_BASE, SMOKE_OUTPUT_BASE);
    conv1d_write_reg(CONV1D_INPUT_LEN, SMOKE_INPUT_LEN);
    conv1d_write_reg(CONV1D_IN_CH, SMOKE_IN_CH);
    conv1d_write_reg(CONV1D_OUT_CH, SMOKE_OUT_CH);
    conv1d_write_reg(CONV1D_QUANT,
                     (SMOKE_OUT_SHIFT & CONV1D_QUANT_SHIFT_MASK) |
                     (SMOKE_RELU_EN ? CONV1D_QUANT_RELU_EN : 0u));
    conv1d_write_output_shift(0, 3u);
    conv1d_write_output_shift(1, 4u);

    if (check_reg("INPUT_BASE", CONV1D_INPUT_BASE, SMOKE_INPUT_BASE) != 0 ||
        check_reg("WEIGHT_BASE", CONV1D_WEIGHT_BASE, SMOKE_WEIGHT_BASE) != 0 ||
        check_reg("BIAS_BASE", CONV1D_BIAS_BASE, SMOKE_BIAS_BASE) != 0 ||
        check_reg("OUTPUT_BASE", CONV1D_OUTPUT_BASE, SMOKE_OUTPUT_BASE) != 0 ||
        check_reg("INPUT_LEN", CONV1D_INPUT_LEN, SMOKE_INPUT_LEN) != 0 ||
        check_reg("IN_CH", CONV1D_IN_CH, SMOKE_IN_CH) != 0 ||
        check_reg("OUT_CH", CONV1D_OUT_CH, SMOKE_OUT_CH) != 0 ||
        check_reg("QUANT_INDEX", CONV1D_QUANT_INDEX, 1u) != 0 ||
        check_reg("QUANT_SHIFT_DATA", CONV1D_QUANT_SHIFT_DATA, 4u) != 0) {
        return -1;
    }

    uart_printf("CONV1D_APB: PASS\r\n");

    /*
     * Do not write CTRL.start in Phase 5A. ID/config readback proves APB
     * reachability without depending on the future scratchpad/AHB memory path.
     */
    return 0;
}

#ifdef CONV1D_SMOKE_TEST_MAIN
int main(void)
{
    uart_init();

    if (conv1d_smoke_test() != 0)
        return 1;

    return 0;
}
#endif
