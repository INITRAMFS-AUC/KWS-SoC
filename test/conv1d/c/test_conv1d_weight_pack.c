/*
 * test_conv1d_weight_pack.c
 *
 * Standalone host unit test for Conv1D weight packing math.
 *
 * Build example:
 *   cc -std=c11 -Wall -Wextra -Itest/conv1d/include \
 *      test/conv1d/c/test_conv1d_weight_pack.c -o /tmp/test_conv1d_weight_pack
 *   /tmp/test_conv1d_weight_pack
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "conv1d_accel_nnom_bridge.h"

static void conv1d_pack_weights_k3_4lane_under_test(
    const int8_t *nnom_weights,
    int8_t *packed_weights,
    int in_ch,
    int out_ch
) {
    int oc;
    int k;
    int ic;
    int channel_groups;
    size_t packed_bytes;

    if (nnom_weights == NULL || packed_weights == NULL || in_ch <= 0 || out_ch <= 0)
        return;

    channel_groups = CONV1D_CHANNEL_GROUPS(in_ch);
    packed_bytes = CONV1D_PACKED_WEIGHT_BYTES(in_ch, out_ch);
    memset(packed_weights, 0, packed_bytes);

    for (oc = 0; oc < out_ch; oc++) {
        for (k = 0; k < 3; k++) {
            for (ic = 0; ic < in_ch; ic++) {
                int group = ic / 4;
                int lane = ic % 4;
                size_t dst = (size_t)((((oc * 3 + k) * channel_groups + group) * 4) + lane);
                size_t src = (size_t)(((k * in_ch + ic) * out_ch) + oc);
                packed_weights[dst] = nnom_weights[src];
            }
        }
    }
}

static int run_case(int in_ch, int out_ch)
{
    size_t src_bytes = (size_t)(3 * in_ch * out_ch);
    size_t packed_bytes = CONV1D_PACKED_WEIGHT_BYTES(in_ch, out_ch);
    int channel_groups = CONV1D_CHANNEL_GROUPS(in_ch);
    int8_t *src = (int8_t *)malloc(src_bytes);
    int8_t *dst = (int8_t *)malloc(packed_bytes);
    int errors = 0;
    int oc;
    int k;
    int group;
    int lane;

    if (src == NULL || dst == NULL) {
        fprintf(stderr, "alloc failed for in_ch=%d out_ch=%d\n", in_ch, out_ch);
        free(src);
        free(dst);
        return 1;
    }

    for (k = 0; k < 3; k++) {
        int ic;
        for (ic = 0; ic < in_ch; ic++) {
            for (oc = 0; oc < out_ch; oc++) {
                size_t src_idx = (size_t)(((k * in_ch + ic) * out_ch) + oc);
                src[src_idx] = (int8_t)((k * 37 + ic * 11 + oc * 3) - 64);
            }
        }
    }

    memset(dst, (int8_t)0x5A, packed_bytes);
    conv1d_pack_weights_k3_4lane_under_test(src, dst, in_ch, out_ch);

    for (oc = 0; oc < out_ch; oc++) {
        for (k = 0; k < 3; k++) {
            for (group = 0; group < channel_groups; group++) {
                for (lane = 0; lane < 4; lane++) {
                    int ic = group * 4 + lane;
                    size_t dst_idx = (size_t)((((oc * 3 + k) * channel_groups + group) * 4) + lane);
                    int8_t exp = 0;
                    if (ic < in_ch) {
                        size_t src_idx = (size_t)(((k * in_ch + ic) * out_ch) + oc);
                        exp = src[src_idx];
                    }

                    if (dst[dst_idx] != exp) {
                        fprintf(stderr,
                                "mismatch in_ch=%d out_ch=%d oc=%d k=%d group=%d lane=%d "
                                "dst_idx=%zu got=%d exp=%d\n",
                                in_ch,
                                out_ch,
                                oc,
                                k,
                                group,
                                lane,
                                dst_idx,
                                (int)dst[dst_idx],
                                (int)exp);
                        errors++;
                    }
                }
            }
        }
    }

    free(src);
    free(dst);
    return errors;
}

int main(void)
{
    int failures = 0;

    failures += run_case(4, 2); /* exact lane fill */
    failures += run_case(5, 3); /* tail padding lane zeros */

    if (failures == 0) {
        printf("PASS: Conv1D weight pack mapping is correct\n");
        return 0;
    }

    printf("FAIL: Conv1D weight pack mapping mismatches=%d\n", failures);
    return 1;
}