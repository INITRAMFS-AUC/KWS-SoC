/*
 * test_conv1d_quant_map.c
 *
 * Standalone host unit test for mapping NNoM Conv2D quantization parameters
 * to the Conv1D accelerator bias memory and QUANT register.
 */

#include <stdint.h>
#include <stdio.h>

#include "conv1d_accel_nnom_bridge.h"

static const int8_t conv2d_1_output_shift[36] = {
    9, 10, 11, 10, 9, 9, 9, 10, 9, 9, 9, 9,
    9, 10, 9, 10, 9, 9, 10, 9, 10, 10, 9, 10,
    9, 10, 9, 9, 9, 10, 10, 9, 10, 9, 10, 9
};

static const int8_t conv2d_1_bias_shift[36] = {
    5, 5, 6, 4, 5, 4, 4, 5, 5, 4, 4, 5,
    3, 2, 4, 5, 4, 2, 4, 4, 3, 3, 4, 4,
    5, 6, 5, 4, 3, 2, 5, 4, 6, 5, 6, 5
};

static const int8_t conv2d_2_output_shift[36] = {
    8, 8, 8, 7, 9, 7, 7, 7, 8, 8, 9, 9,
    7, 10, 9, 8, 9, 7, 8, 8, 8, 8, 8, 8,
    8, 9, 9, 7, 8, 9, 8, 8, 9, 8, 8, 7
};

static const int8_t conv2d_2_bias_shift[36] = {
    4, 5, 2, 4, 0, 4, 3, 2, 4, 3, 5, 0,
    3, 5, 3, 3, 4, 0, 5, 4, 3, 5, 4, 6,
    4, 4, 2, 3, 5, 5, 6, 3, 5, 5, 4, 4
};

static const int8_t conv2d_3_output_shift[36] = {
    6, 6, 7, 6, 6, 6, 6, 7, 6, 7, 7, 6,
    7, 6, 6, 7, 6, 6, 6, 6, 7, 7, 6, 7,
    6, 6, 6, 6, 7, 7, 6, 6, 6, 6, 7, 6
};

static const int8_t conv2d_3_bias_shift[36] = {
    2, 2, 4, 3, 2, 2, 2, 5, 3, 4, 4, 0,
    5, 2, 2, 3, 1, 2, 2, 3, 1, 4, 3, 3,
    3, 2, 4, 3, 1, 4, 1, 1, 3, 0, 1, 4
};

static int failures;

static void expect_i32(const char *name, int32_t got, int32_t exp)
{
    if (got != exp) {
        printf("FAIL: %s got=%ld exp=%ld\n",
               name, (long)got, (long)exp);
        failures++;
    }
}

static void expect_u32(const char *name, uint32_t got, uint32_t exp)
{
    if (got != exp) {
        printf("FAIL: %s got=0x%lx exp=0x%lx\n",
               name, (unsigned long)got, (unsigned long)exp);
        failures++;
    }
}

static int is_uniform(const int8_t *values, int count)
{
    int i;

    for (i = 1; i < count; i++) {
        if (values[i] != values[0])
            return 0;
    }

    return 1;
}

static void test_shift_table(const char *name,
                             const int8_t *output_shift,
                             const int8_t *bias_shift,
                             int count)
{
    int i;

    for (i = 0; i < count; i++) {
        if (output_shift[i] < 0 || output_shift[i] > 31) {
            printf("FAIL: %s output_shift[%d]=%d out of QUANT range\n",
                   name, i, output_shift[i]);
            failures++;
        }

        if (bias_shift[i] < 0 || bias_shift[i] > 30) {
            printf("FAIL: %s bias_shift[%d]=%d out of test range\n",
                   name, i, bias_shift[i]);
            failures++;
        }

        expect_u32("quant field preserves output shift",
                   conv1d_make_quant_reg(output_shift[i], 0),
                   (uint32_t)output_shift[i]);
    }

    if (is_uniform(output_shift, count)) {
        printf("INFO: %s output shifts are uniform\n", name);
    } else {
        printf("INFO: %s output shifts are per-axis/non-uniform\n", name);
    }
}

static void test_bias_helpers(void)
{
    const int32_t src[4] = {3, -4, 0, 127};
    int32_t dst[4] = {0, 0, 0, 0};

    conv1d_prepare_bias(src, dst, 4, 5);
    expect_i32("bias << shift positive", dst[0], 96);
    expect_i32("bias << shift negative", dst[1], -128);
    expect_i32("bias << shift zero", dst[2], 0);
    expect_i32("bias << shift large", dst[3], 4064);

    conv1d_prepare_bias_with_rounding(src, dst, 4, 5, 9);
    expect_i32("bias pre-shift adds NNoM round", dst[0], 352);
    expect_i32("negative bias pre-shift adds NNoM round", dst[1], 128);
}

static void test_quant_helpers(void)
{
    expect_u32("quant relu disabled", conv1d_make_quant_reg(9, 0), 9u);
    expect_u32("quant relu enabled",
               conv1d_make_quant_reg(10, 1),
               CONV1D_QUANT_RELU_EN | 10u);

    expect_i32("positive rounded quantize",
               conv1d_reference_quantize(1000, 3, 0),
               125);
    expect_i32("negative rounded quantize",
               conv1d_reference_quantize(-1000, 3, 0),
               -125);
    expect_i32("positive saturation",
               conv1d_reference_quantize(200000, 4, 0),
               127);
    expect_i32("negative saturation",
               conv1d_reference_quantize(-200000, 4, 0),
               -128);
    expect_i32("relu clamps negative after shift",
               conv1d_reference_quantize(-1000, 3, 1),
               0);
}

static int8_t nnom_reference_from_parts(int32_t bias,
                                        int8_t bias_shift,
                                        int32_t raw_sum,
                                        int8_t output_shift,
                                        int relu_en)
{
    int32_t prepared = conv1d_saturate_i32(
        (int64_t)conv1d_shift_bias_value(bias, bias_shift) +
        conv1d_nnom_round_term(output_shift));

    return conv1d_quantize_prepared_acc(
        conv1d_saturate_i32((int64_t)prepared + raw_sum),
        output_shift,
        relu_en);
}

static void test_per_oc_prepared_bias_and_shift(void)
{
    const int out_ch = 4;
    const int32_t nnom_bias[4] = {3, -4, 5, -6};
    const int8_t bias_shift[4] = {5, 3, 2, 4};
    const int8_t output_shift[4] = {3, 4, 5, 6};
    const int32_t raw_sum[4] = {512, -256, 200000, -200000};
    int32_t accel_bias[4] = {0, 0, 0, 0};
    uint8_t accel_shift[4] = {0, 0, 0, 0};
    int oc;

    conv1d_prepare_bias_and_shifts(nnom_bias,
                                   bias_shift,
                                   output_shift,
                                   accel_bias,
                                   accel_shift,
                                   out_ch);

    expect_i32("per-oc prepared bias ch0", accel_bias[0], (3 << 5) + 4);
    expect_i32("per-oc prepared bias ch1", accel_bias[1], -32 + 8);
    expect_i32("per-oc shift ch2", accel_shift[2], 5);
    expect_i32("per-oc shift ch3", accel_shift[3], 6);

    for (oc = 0; oc < out_ch; oc++) {
        int8_t exp = nnom_reference_from_parts(nnom_bias[oc],
                                               bias_shift[oc],
                                               raw_sum[oc],
                                               output_shift[oc],
                                               0);
        int8_t got = conv1d_quantize_prepared_acc(
            conv1d_saturate_i32((int64_t)accel_bias[oc] + raw_sum[oc]),
            accel_shift[oc],
            0);
        expect_i32("per-oc accelerator formula matches NNoM formula", got, exp);
    }

    expect_i32("per-oc positive accumulator",
               conv1d_quantize_prepared_acc(accel_bias[0] + raw_sum[0],
                                            accel_shift[0], 0),
               76);
    expect_i32("per-oc negative accumulator",
               conv1d_quantize_prepared_acc(accel_bias[1] + raw_sum[1],
                                            accel_shift[1], 0),
               -18);
    expect_i32("per-oc saturation high",
               conv1d_quantize_prepared_acc(accel_bias[2] + raw_sum[2],
                                            accel_shift[2], 0),
               127);
    expect_i32("per-oc saturation low",
               conv1d_quantize_prepared_acc(accel_bias[3] + raw_sum[3],
                                            accel_shift[3], 0),
               -128);
}

int main(void)
{
    test_shift_table("conv2d_1", conv2d_1_output_shift, conv2d_1_bias_shift, 36);
    test_shift_table("conv2d_2", conv2d_2_output_shift, conv2d_2_bias_shift, 36);
    test_shift_table("conv2d_3", conv2d_3_output_shift, conv2d_3_bias_shift, 36);
    test_bias_helpers();
    test_quant_helpers();
    test_per_oc_prepared_bias_and_shift();

    if (failures == 0) {
        printf("PASS: Conv1D quantization mapping helpers are correct\n");
        return 0;
    }

    printf("FAIL: Conv1D quantization mapping failures=%d\n", failures);
    return 1;
}
