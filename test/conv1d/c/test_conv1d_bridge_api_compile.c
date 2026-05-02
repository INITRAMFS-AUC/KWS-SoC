/*
 * test_conv1d_bridge_api_compile.c
 *
 * Host-side API test for the Conv1D accelerator bridge APB call path.
 * Built with CONV1D_ACCEL_HOST_TEST so MMIO writes target fake registers.
 */

#include <stdint.h>
#include <stdio.h>

#include "conv1d_accel_nnom_bridge.h"

static int failures;

static void expect_int(const char *name, int got, int exp)
{
    if (got != exp) {
        printf("FAIL: %s got=%d exp=%d\n", name, got, exp);
        failures++;
    }
}

static void test_null_args_fallback(void)
{
    conv1d_accel_stats_reset();

    expect_int("null input returns fallback",
               conv1d_accel_run_prepared_layer(NULL, NULL, NULL, NULL,
                                               NULL, 8, 4, 2),
               0);
    expect_int("null args increment unsupported",
               (int)g_conv1d_accel_stats.conv1d_hw_unsupported,
               1);
}

static void test_unsupported_dims_fallback(void)
{
    union {
        uint32_t align;
        int8_t data[32];
    } input = {0}, weights = {0}, output = {0};
    int32_t bias[4] = {0};
    uint8_t shifts[4] = {3, 4, 5, 6};

    conv1d_accel_stats_reset();

    expect_int("short input_len returns fallback",
               conv1d_accel_run_prepared_layer(input.data,
                                               weights.data,
                                               bias,
                                               shifts,
                                               output.data,
                                               2,
                                               4,
                                               4),
               0);
    expect_int("bad dims increment unsupported",
               (int)g_conv1d_accel_stats.conv1d_hw_unsupported,
               1);
}

static void test_valid_host_stub_call(void)
{
    union {
        uint32_t align;
        int8_t data[64];
    } input = {0}, weights = {0}, output = {0};
    const int32_t nnom_bias[4] = {3, -4, 5, -6};
    const int8_t bias_shift[4] = {5, 3, 2, 4};
    const int8_t output_shift[4] = {3, 4, 5, 6};
    int32_t prepared_bias[4] = {0};
    uint8_t prepared_shift[4] = {0};

    conv1d_accel_stats_reset();
    conv1d_prepare_bias_and_shifts(nnom_bias,
                                   bias_shift,
                                   output_shift,
                                   prepared_bias,
                                   prepared_shift,
                                   4);

    expect_int("prepared shift visible", prepared_shift[2], 5);
    expect_int("prepared bias includes rounding", prepared_bias[0], (3 << 5) + 4);

    expect_int("valid host-stub APB call returns success",
               conv1d_accel_run_prepared_layer(input.data,
                                               weights.data,
                                               prepared_bias,
                                               prepared_shift,
                                               output.data,
                                               8,
                                               4,
                                               4),
               1);
    expect_int("valid call increments hw_calls",
               (int)g_conv1d_accel_stats.conv1d_hw_calls,
               1);
    expect_int("valid call has no timeout",
               (int)g_conv1d_accel_stats.conv1d_hw_timeouts,
               0);
}

int main(void)
{
    test_null_args_fallback();
    test_unsupported_dims_fallback();
    test_valid_host_stub_call();

    if (failures == 0) {
        printf("PASS: Conv1D bridge APB API host test passed\n");
        return 0;
    }

    printf("FAIL: Conv1D bridge APB API failures=%d\n", failures);
    return 1;
}
