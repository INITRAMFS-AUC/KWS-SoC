/**
 * conv1d_accel_nnom_bridge.c
 *
 * Software bridge implementation. Provides layer callback and accelerator
 * control logic for integrating the Conv1D accelerator into NNoM's model_run().
 *
 * Implementation Phases:
 *   Phase 1: Layer type/size verification and stats collection (this version)
 *   Phase 2: Weight format conversion (online transpose)
 *   Phase 3: Quantization parameter mapping
 *   Phase 4: Full acceleration with fallback
 */

#include <stdint.h>
#include <string.h>

#include "nnom.h"
#include "nnom_layers.h"
#include "nnom_local.h"

#include "conv1d_accel_regs.h"
#include "conv1d_accel_nnom_bridge.h"

/* Global statistics */
conv1d_accel_stats_t g_conv1d_accel_stats = {0};

/**
 * Helper: Check if layer is a 1D convolution (height=1, kernel_width=3).
 */
static int is_conv1d_k3(nnom_layer_t *layer) {
    if (layer->type != NNOM_CONV_2D)
        return 0;

    nnom_conv2d_layer_t *cl = (nnom_conv2d_layer_t *)layer;

    if (cl->kernel.h != 1 || cl->kernel.w != 3)
        return 0;

    if (cl->stride.h != 1 || cl->stride.w != 1)
        return 0;

    return 1;
}

/**
 * Helper: Get dimensions and pointers from layer.
 */
typedef struct {
    int input_len;
    int in_ch;
    int out_ch;
    uint8_t *input;
    uint8_t *output;
    uint8_t *weights;
    uint8_t *bias;
} conv1d_config_t;

static int get_layer_config(nnom_layer_t *layer, conv1d_config_t *cfg) {
    if (layer->in == NULL || layer->out == NULL)
        return 0;

    nnom_tensor_t *in_tensor = layer->in->tensor;
    nnom_tensor_t *out_tensor = layer->out->tensor;
    nnom_conv2d_layer_t *cl = (nnom_conv2d_layer_t *)layer;

    if (in_tensor == NULL || out_tensor == NULL)
        return 0;

    cfg->input_len = in_tensor->dim[1];
    cfg->in_ch = in_tensor->dim[2];
    cfg->out_ch = out_tensor->dim[2];
    cfg->input = (uint8_t *)in_tensor->p_data;
    cfg->output = (uint8_t *)out_tensor->p_data;

    if (cl->weight == NULL || cl->bias == NULL)
        return 0;

    cfg->weights = (uint8_t *)cl->weight->p_data;
    cfg->bias = (uint8_t *)cl->bias->p_data;

    return 1;
}

/**
 * Helper: Check if pointers are 4-byte aligned (DMA requirement).
 */
static int is_aligned_4(const void *ptr) {
    return (((uintptr_t)ptr) & 0x3) == 0;
}

static int has_valid_quant_shifts(const nnom_conv2d_layer_t *cl, int out_ch)
{
    int oc;

    if (cl == NULL || cl->output_rshift == NULL || cl->bias_lshift == NULL ||
        out_ch <= 0)
        return 0;

    for (oc = 0; oc < out_ch; oc++) {
        if (cl->output_rshift[oc] < 0 || cl->output_rshift[oc] > 31)
            return 0;
        if (cl->bias_lshift[oc] < 0 || cl->bias_lshift[oc] > 30)
            return 0;
    }

    return 1;
}

/**
 * Main accelerator control: Try to run layer on accelerator.
 */
int conv1d_accel_try_run_layer(nnom_layer_t *layer) {
    if (layer == NULL)
        return 0;

    /* Phase 1: Layer type and kernel size check */
    if (!is_conv1d_k3(layer))
        return 0;

    /* Phase 1: Dimension and pointer extraction */
    conv1d_config_t cfg;
    memset(&cfg, 0, sizeof(cfg));

    if (!get_layer_config(layer, &cfg))
        return 0;

    /* Phase 1: Validate channel counts */
    if (cfg.in_ch < 1 || cfg.in_ch > 64)
        return 0;

    if (cfg.out_ch < 1 || cfg.out_ch > 255)
        return 0;

    if (cfg.input_len < 1)
        return 0;

    /* Phase 1: Validate quantization formats */
    if (layer->in->tensor->bitwidth != 8 ||
        layer->out->tensor->bitwidth != 8)
        return 0;

    /* Phase 1: Check pointer alignment */
    if (!is_aligned_4(cfg.input) ||
        !is_aligned_4(cfg.output) ||
        !is_aligned_4(cfg.weights) ||
        !is_aligned_4(cfg.bias))
        return 0;

    /*
     * Phase 3: per-output-channel shifts are now representable in the
     * APB/RTL interface. Live acceleration still waits for Phase 4 APB setup
     * and memory-path wiring, so software fallback remains active.
     */
    if (!has_valid_quant_shifts((nnom_conv2d_layer_t *)layer, cfg.out_ch))
        return 0;

    /* Phase 4: Implement APB setup and accelerator invocation. */

    return 0;  /* Skip acceleration for now. */
}

/**
 * NNoM layer callback: invoked after each layer executes during model_run().
 */
nnom_status_t conv1d_accel_layer_callback(
    nnom_model_t *m,
    nnom_layer_t *layer
) {
    (void)m;

    g_conv1d_accel_stats.layer_callbacks++;

    if (layer->type == NNOM_CONV_2D && is_conv1d_k3(layer)) {
        g_conv1d_accel_stats.conv1d_attempts++;

        if (conv1d_accel_try_run_layer(layer)) {
            g_conv1d_accel_stats.conv1d_success++;
        } else {
            g_conv1d_accel_stats.conv1d_skip++;
        }
    }

    return NN_SUCCESS;
}
