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
 *   2. Configure a tiny Conv1D layer
 *   3. Start the accelerator and poll done
 *   4. (Optional) Compare output bytes against a software reference
 */

#include <stdint.h>
#include "conv1d_accel_regs.h"

/* The firmware should provide a print primitive. Replace as appropriate. */
extern int  uart_printf(const char *fmt, ...);
extern void panic(const char *msg);

/*
 * A tiny test layer: input_len=8, in_ch=4, out_ch=2, out_shift=0, relu off.
 * The exact tensor data is the firmware's job. The addresses below are
 * placeholder offsets in a contiguous SRAM scratchpad mapped at
 * CONV1D_DATA_BASE; adjust to your SoC's memory layout.
 */
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

/*
 * Caller fills SMOKE_INPUT_BASE / SMOKE_WEIGHT_BASE / SMOKE_BIAS_BASE
 * before calling this routine. Output bytes appear at SMOKE_OUTPUT_BASE.
 */
int conv1d_smoke_test(void)
{
    /* 1. ID check */
    uint32_t id = conv1d_read_reg(CONV1D_ID);
    if (id != CONV1D_EXPECTED_ID) {
        uart_printf("conv1d: ID mismatch: got 0x%08x expected 0x%08x\r\n",
                    id, CONV1D_EXPECTED_ID);
        return -1;
    }
    uart_printf("conv1d: ID = 0x%08x OK\r\n", id);

    /* 2-4. Configure, start, and poll done */
    int rc = conv1d_run_layer(SMOKE_INPUT_BASE,
                              SMOKE_WEIGHT_BASE,
                              SMOKE_BIAS_BASE,
                              SMOKE_OUTPUT_BASE,
                              SMOKE_INPUT_LEN,
                              SMOKE_IN_CH,
                              SMOKE_OUT_CH,
                              SMOKE_OUT_SHIFT,
                              SMOKE_RELU_EN);
    if (rc != 0) {
        uart_printf("conv1d: run_layer returned %d\r\n", rc);
        return rc;
    }

    uart_printf("conv1d: smoke layer completed (done observed)\r\n");

    /*
     * 5. (Optional) Compare output bytes against a software-computed
     *    reference. The reference computation is typically generated
     *    offline and embedded into the firmware as a const array.
     *    Skipped here; the firmware integrator should add this once the
     *    SRAM-backed memory path is wired up.
     */
    return 0;
}

#ifdef CONV1D_SMOKE_TEST_MAIN
int main(void)
{
    if (conv1d_smoke_test() != 0) {
        panic("conv1d smoke test failed");
    }
    return 0;
}
#endif
