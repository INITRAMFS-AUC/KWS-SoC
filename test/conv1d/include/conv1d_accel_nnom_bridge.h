/**
 * conv1d_accel_nnom_bridge.h
 *
 * Software bridge between NNoM Conv2D layers and the Conv1D accelerator.
 * Enables transparent acceleration of 1D convolutions (kernel_size {1,3})
 * within the NNoM inference pipeline.
 */

#ifndef CONV1D_ACCEL_NNOM_BRIDGE_H
#define CONV1D_ACCEL_NNOM_BRIDGE_H

#include <stddef.h>
#include <stdint.h>

/* Forward declare NNoM types when nnom.h is not already included. */
#ifndef __NNOM_H__
typedef struct _nnom_model nnom_model_t;
typedef struct _nnom_layer_t nnom_layer_t;
typedef int nnom_status_t;
#endif

/* Conv1D accelerator packs channels in groups of 4 int8 lanes. */
#define CONV1D_ACCEL_KERNEL_SIZE 3
#define CONV1D_ACCEL_MAX_IN_CH 64
#define CONV1D_ACCEL_MAX_OUT_CH 64
#define CONV1D_CHANNEL_GROUPS(in_ch) (((in_ch) + 3) / 4)
#define CONV1D_PACKED_WEIGHT_BYTES(in_ch, out_ch) \
    ((size_t)(out_ch) * 3u * (size_t)CONV1D_CHANNEL_GROUPS(in_ch) * 4u)

#ifndef CONV1D_QUANT_SHIFT_MASK
#define CONV1D_QUANT_SHIFT_MASK 0x1Fu
#endif

#ifndef CONV1D_QUANT_RELU_EN
#define CONV1D_QUANT_RELU_EN (1u << 5)
#endif

static inline int32_t conv1d_saturate_i32(int64_t value)
{
    if (value > INT32_MAX)
        return INT32_MAX;
    if (value < INT32_MIN)
        return INT32_MIN;
    return (int32_t)value;
}

static inline int8_t conv1d_saturate_i8(int32_t value)
{
    if (value > 127)
        return 127;
    if (value < -128)
        return -128;
    return (int8_t)value;
}

static inline int32_t conv1d_nnom_round_term(int output_shift)
{
#ifdef NNOM_TRUNCATE
    (void)output_shift;
    return 0;
#else
    if (output_shift <= 0)
        return 0;
    return (int32_t)(1u << (output_shift - 1));
#endif
}

static inline int32_t conv1d_shift_bias_value(int32_t bias, int bias_shift)
{
    if (bias_shift <= 0)
        return bias;
    return conv1d_saturate_i32((int64_t)bias * (INT64_C(1) << bias_shift));
}

static inline void conv1d_prepare_bias(const int32_t *nnom_bias,
                                       int32_t *accel_bias,
                                       int out_ch,
                                       int bias_shift)
{
    int oc;

    if (nnom_bias == NULL || accel_bias == NULL || out_ch <= 0)
        return;

    for (oc = 0; oc < out_ch; oc++)
        accel_bias[oc] = conv1d_shift_bias_value(nnom_bias[oc], bias_shift);
}

static inline void conv1d_prepare_bias_with_rounding(const int32_t *nnom_bias,
                                                     int32_t *accel_bias,
                                                     int out_ch,
                                                     int bias_shift,
                                                     int output_shift)
{
    int oc;
    int32_t round = conv1d_nnom_round_term(output_shift);

    if (nnom_bias == NULL || accel_bias == NULL || out_ch <= 0)
        return;

    for (oc = 0; oc < out_ch; oc++) {
        int32_t shifted = conv1d_shift_bias_value(nnom_bias[oc], bias_shift);
        accel_bias[oc] = conv1d_saturate_i32((int64_t)shifted + round);
    }
}

static inline void conv1d_prepare_bias_and_shifts(const int32_t *nnom_bias,
                                                  const int8_t *bias_shift,
                                                  const int8_t *output_shift,
                                                  int32_t *accel_bias,
                                                  uint8_t *accel_shift,
                                                  int out_ch)
{
    int oc;

    if (nnom_bias == NULL || bias_shift == NULL || output_shift == NULL ||
        accel_bias == NULL || accel_shift == NULL || out_ch <= 0)
        return;

    for (oc = 0; oc < out_ch; oc++) {
        int out_s = output_shift[oc];
        int32_t shifted = conv1d_shift_bias_value(nnom_bias[oc], bias_shift[oc]);
        accel_bias[oc] = conv1d_saturate_i32((int64_t)shifted +
                                             conv1d_nnom_round_term(out_s));
        accel_shift[oc] = (uint8_t)out_s;
    }
}

static inline uint32_t conv1d_make_quant_reg(int output_shift, int relu_en)
{
    return ((uint32_t)output_shift & CONV1D_QUANT_SHIFT_MASK) |
           (relu_en ? CONV1D_QUANT_RELU_EN : 0u);
}

static inline int8_t conv1d_quantize_prepared_acc(int32_t prepared_acc,
                                                  int output_shift,
                                                  int relu_en)
{
    int32_t shifted = (output_shift > 0) ?
                      (prepared_acc >> output_shift) :
                      prepared_acc;

    if (relu_en && shifted < 0)
        shifted = 0;

    return conv1d_saturate_i8(shifted);
}

static inline int8_t conv1d_reference_quantize(int32_t acc,
                                               int output_shift,
                                               int relu_en)
{
    int32_t rounded;
    int32_t shifted;

    rounded = conv1d_saturate_i32((int64_t)acc +
                                  conv1d_nnom_round_term(output_shift));
    shifted = (output_shift > 0) ? (rounded >> output_shift) : rounded;

    if (relu_en && shifted < 0)
        shifted = 0;

    return conv1d_saturate_i8(shifted);
}

/**
 * Statistics tracking for Conv1D accelerator usage.
 */
typedef struct {
    uint32_t layer_callbacks;
    uint32_t conv1d_attempts;
    uint32_t conv1d_success;
    uint32_t conv1d_skip;
    uint32_t conv1d_hw_calls;
    uint32_t conv1d_hw_timeouts;
    uint32_t conv1d_hw_unsupported;
    uint32_t conv1d_hw_alignment_fail;
    uint32_t cycles_accel_total;
} conv1d_accel_stats_t;

/**
 * Global statistics (read-only from caller perspective).
 */
extern conv1d_accel_stats_t g_conv1d_accel_stats;

/**
 * Reset statistics counters.
 */
static inline void conv1d_accel_stats_reset(void)
{
    g_conv1d_accel_stats.layer_callbacks = 0;
    g_conv1d_accel_stats.conv1d_attempts = 0;
    g_conv1d_accel_stats.conv1d_success = 0;
    g_conv1d_accel_stats.conv1d_skip = 0;
    g_conv1d_accel_stats.conv1d_hw_calls = 0;
    g_conv1d_accel_stats.conv1d_hw_timeouts = 0;
    g_conv1d_accel_stats.conv1d_hw_unsupported = 0;
    g_conv1d_accel_stats.conv1d_hw_alignment_fail = 0;
    g_conv1d_accel_stats.cycles_accel_total = 0;
}

/**
 * Run a prepared K=3 Conv1D layer through the APB accelerator path.
 * Inputs must already use accelerator-compatible memory layout:
 * - packed_weights: [out_ch][3][ceil(in_ch/4)][4]
 * - prepared_bias: (NNoM bias << bias_shift) + NNOM_ROUND(output_shift)
 * - output_shifts: one output_shift per output channel
 *
 * Returns 1 on accelerator completion, 0 when firmware should fall back.
 */
int conv1d_accel_run_prepared_layer(const int8_t *input,
                                    const int8_t *packed_weights,
                                    const int32_t *prepared_bias,
                                    const uint8_t *output_shifts,
                                    int8_t *output,
                                    int input_len,
                                    int in_ch,
                                    int out_ch);

/**
 * Attempt to accelerate a Conv1D layer using the hardware accelerator.
 * Returns 1 if accelerator was used, 0 if skipped (software fallback).
 */
int conv1d_accel_try_run_layer(nnom_layer_t *layer);

/**
 * Layer callback for NNoM model_run(). Register via model_set_callback().
 * Invoked after each layer completes.
 */
nnom_status_t conv1d_accel_layer_callback(nnom_model_t *m, nnom_layer_t *layer);

#endif /* CONV1D_ACCEL_NNOM_BRIDGE_H */
