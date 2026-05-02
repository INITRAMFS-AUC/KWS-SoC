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

/* Forward declare NNoM types (avoid circular includes). */
typedef struct _nnom_model_t nnom_model_t;
typedef struct _nnom_layer_t nnom_layer_t;
typedef int nnom_status_t;

/* Conv1D accelerator packs channels in groups of 4 int8 lanes. */
#define CONV1D_CHANNEL_GROUPS(in_ch) (((in_ch) + 3) / 4)
#define CONV1D_PACKED_WEIGHT_BYTES(in_ch, out_ch) \
    ((size_t)(out_ch) * 3u * (size_t)CONV1D_CHANNEL_GROUPS(in_ch) * 4u)

/**
 * Statistics tracking for Conv1D accelerator usage.
 */
typedef struct {
    uint32_t layer_callbacks;
    uint32_t conv1d_attempts;
    uint32_t conv1d_success;
    uint32_t conv1d_skip;
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
    g_conv1d_accel_stats.cycles_accel_total = 0;
}

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
