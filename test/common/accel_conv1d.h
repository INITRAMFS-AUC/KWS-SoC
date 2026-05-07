/* accel_conv1d.h — firmware driver for the conv1d hardware accelerator.
 *
 * Accelerator APB base: 0x4000_C000
 * Register map mirrors conv1d_accel.v exactly.
 *
 * Usage:
 *   accel_conv1d(src, wt, dst, bias32, shift_arr, c_in, c_out, k_w, stride, w_in);
 *   Blocks until done. Input/output/bias must be in SRAM; weights may be in flash.
 *   bias32[] must be pre-scaled: bias32[c] = (int32)(bias8[c] << bias_lshift[c]) + round
 *   shift_arr[] is per-channel uint8 right-shift (from NNoM output_rshift).
 */

#ifndef ACCEL_CONV1D_H
#define ACCEL_CONV1D_H

#include <stdint.h>

#define ACCEL_BASE       0x4000C000UL
#define ACCEL_REG(off)   (*(volatile uint32_t *)(ACCEL_BASE + (off)))

#define ACCEL_CTRL       ACCEL_REG(0x00)
#define ACCEL_SRC_ADDR   ACCEL_REG(0x04)
#define ACCEL_WT_ADDR    ACCEL_REG(0x08)
#define ACCEL_DST_ADDR   ACCEL_REG(0x0C)
#define ACCEL_BS_ADDR    ACCEL_REG(0x10)
#define ACCEL_CFG0       ACCEL_REG(0x14)
#define ACCEL_CFG1       ACCEL_REG(0x18)
#define ACCEL_SHIFT      ACCEL_REG(0x1C)   /* legacy scalar shift */
#define ACCEL_SHIFT_ADDR ACCEL_REG(0x20)   /* per-channel shift byte array pointer */

/* NNoM-aware XIP-cache prefetch hint side-band.
 *
 * Writing PREFETCH_BASE/LEN, then setting PREFETCH_CTRL=1, asks the
 * XIP cache to pre-fetch the next weight scan into a separate single-
 * line victim buffer.  When PREFETCH_CTRL=0 (reset default), the
 * cache is byte-for-byte identical to a no-prefetch build.  See
 * peris/xip/DESIGN.md §"NNoM-aware cache" for the full contract.
 *
 * Today's MVP only fetches the FIRST line at PREFETCH_BASE; LEN is
 * latched but not consumed by the FSM yet.  Wiring LEN now so the
 * firmware contract is forward-compatible with the multi-line walk
 * that lands next. */
#define ACCEL_PREFETCH_CTRL  ACCEL_REG(0x24)
#define ACCEL_PREFETCH_BASE  ACCEL_REG(0x28)
#define ACCEL_PREFETCH_LEN   ACCEL_REG(0x2C)

#define ACCEL_PREFETCH_EN    (1u << 0)

#define ACCEL_CTRL_START (1u << 0)
#define ACCEL_CTRL_BUSY  (1u << 8)
#define ACCEL_CTRL_DONE  (1u << 9)

/* Compile-time gate for the NNoM-aware prefetch hint.  Default OFF
 * so any build that doesn't explicitly opt in keeps the XIP cache
 * exactly as it was — we still need the hint API to bake this
 * gating-contract intent into the source.  Pass
 * -DKWS_XIP_PREFETCH=1 to enable. */
#ifndef KWS_XIP_PREFETCH
#define KWS_XIP_PREFETCH 0
#endif

/* Run a Conv1D layer on the hardware accelerator.
 *
 * Computes valid-padding convolution (pre-pad for same-padding):
 *   out[w][c_out] = clip((sum_{k,cin} in[w*stride+k][cin] * wt[c_out][k][cin]
 *                         + bias32[c_out]) >> shift_arr[c_out],  -128, 127)
 *
 * Parameters:
 *   src       — int8_t input  [W_in][C_in]          (SRAM, word-aligned)
 *   wt        — int8_t weight [C_out][K_w][C_in]    (SRAM or flash)
 *   dst       — int8_t output [W_out][C_out]         (SRAM, word-aligned)
 *   bias32    — int32_t pre-scaled bias [C_out]      (SRAM)
 *               bias32[c] = (int8_bias[c] << bias_lshift[c]) + NNOM_ROUND(shift_arr[c])
 *   shift_arr — uint8_t per-channel output right-shift [C_out]  (SRAM)
 *   c_in      — input channels
 *   c_out     — output channels
 *   k_w       — kernel width
 *   stride    — convolution stride
 *   w_in      — input width  (W_out = (W_in - K_w) / stride + 1)
 */
static inline void accel_conv1d(
    const int8_t   *src,
    const int8_t   *wt,
          int8_t   *dst,
    const int32_t  *bias32,
    const uint8_t  *shift_arr,
    uint8_t  c_in,
    uint8_t  c_out,
    uint8_t  k_w,
    uint8_t  stride,
    uint16_t w_in)
{
    ACCEL_SRC_ADDR   = (uint32_t)src;
    ACCEL_WT_ADDR    = (uint32_t)wt;
    ACCEL_DST_ADDR   = (uint32_t)dst;
    ACCEL_BS_ADDR    = (uint32_t)bias32;
    ACCEL_SHIFT_ADDR = (uint32_t)shift_arr;
    ACCEL_CFG0       = ((uint32_t)stride << 24) | ((uint32_t)k_w << 16)
                     | ((uint32_t)c_out  <<  8) | (uint32_t)c_in;
    ACCEL_CFG1       = (uint32_t)w_in;

#if KWS_XIP_PREFETCH
    /* Hint the XIP cache to prefetch the weight scan.  Total weight
     * tensor bytes for this layer = c_out * k_w * c_in (int8_t).
     * Fire the hint just BEFORE start so the cache has the longest
     * possible head-start while the accel walks SRAM for the input
     * patch.  PREFETCH_CTRL is pulsed (write 1, then later 0 isn't
     * needed — the FSM clears pf_pending after dispatch). */
    ACCEL_PREFETCH_BASE = (uint32_t)wt;
    ACCEL_PREFETCH_LEN  = (uint32_t)c_out * (uint32_t)k_w * (uint32_t)c_in;
    ACCEL_PREFETCH_CTRL = ACCEL_PREFETCH_EN;
#endif

    ACCEL_CTRL       = ACCEL_CTRL_START;          /* start */
    while (!(ACCEL_CTRL & ACCEL_CTRL_DONE));      /* poll done */

#if KWS_XIP_PREFETCH
    /* Drop the enable so the next call's rising edge re-arms the FSM
     * with a fresh BASE/LEN.  Without this, prefetch_en stays high
     * and the rising-edge detector in ro_dmc never fires for layer
     * 2+. */
    ACCEL_PREFETCH_CTRL = 0;
#endif
}

#endif /* ACCEL_CONV1D_H */
