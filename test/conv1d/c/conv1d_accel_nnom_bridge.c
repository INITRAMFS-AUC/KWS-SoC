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

#ifdef CONV1D_ACCEL_HOST_TEST
#define __NNOM_H__
#define NNOM_CONV_2D 1
#define NN_SUCCESS 0

typedef int nnom_status_t;
typedef struct _nnom_model nnom_model_t;

typedef struct {
    int bitwidth;
    int dim[3];
    void *p_data;
} nnom_tensor_t;

typedef struct {
    nnom_tensor_t *tensor;
} nnom_layer_io_t;

typedef struct _nnom_layer_t {
    int type;
    nnom_layer_io_t *in;
    nnom_layer_io_t *out;
} nnom_layer_t;

typedef struct {
    void *p_data;
} nnom_blob_t;

typedef struct {
    nnom_layer_t super;
    struct {
        int h;
        int w;
    } kernel;
    struct {
        int h;
        int w;
    } stride;
    nnom_blob_t *weight;
    nnom_blob_t *bias;
    int8_t *output_rshift;
    int8_t *bias_lshift;
} nnom_conv2d_layer_t;
#else
#include "nnom.h"
#include "nnom_layers.h"
#include "nnom_local.h"
#endif

#include "conv1d_accel_regs.h"
#include "conv1d_accel_nnom_bridge.h"

#ifndef CONV1D_ACCEL_TIMEOUT
#define CONV1D_ACCEL_TIMEOUT 1000000u
#endif

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

static int has_valid_output_shifts(const uint8_t *output_shifts, int out_ch)
{
    int oc;

    if (output_shifts == NULL || out_ch <= 0)
        return 0;

    for (oc = 0; oc < out_ch; oc++) {
        if (output_shifts[oc] > CONV1D_QUANT_SHIFT_MASK)
            return 0;
    }

    return 1;
}

static uint32_t ptr_to_accel_addr(const void *ptr)
{
    return (uint32_t)(uintptr_t)ptr;
}

int conv1d_accel_run_prepared_layer(const int8_t *input,
                                    const int8_t *packed_weights,
                                    const int32_t *prepared_bias,
                                    const uint8_t *output_shifts,
                                    int8_t *output,
                                    int input_len,
                                    int in_ch,
                                    int out_ch)
{
    uint32_t timeout;

    if (input == NULL || packed_weights == NULL || prepared_bias == NULL ||
        output_shifts == NULL || output == NULL) {
        g_conv1d_accel_stats.conv1d_hw_unsupported++;
        return 0;
    }

    if (input_len < CONV1D_ACCEL_KERNEL_SIZE ||
        in_ch < 1 || in_ch > CONV1D_ACCEL_MAX_IN_CH ||
        out_ch < 1 || out_ch > CONV1D_ACCEL_MAX_OUT_CH ||
        !has_valid_output_shifts(output_shifts, out_ch)) {
        g_conv1d_accel_stats.conv1d_hw_unsupported++;
        return 0;
    }

    if (!is_aligned_4(input) ||
        !is_aligned_4(packed_weights) ||
        !is_aligned_4(prepared_bias) ||
        !is_aligned_4(output)) {
        g_conv1d_accel_stats.conv1d_hw_alignment_fail++;
        return 0;
    }

    g_conv1d_accel_stats.conv1d_hw_calls++;

    conv1d_write_reg(CONV1D_INPUT_BASE, ptr_to_accel_addr(input));
    conv1d_write_reg(CONV1D_WEIGHT_BASE, ptr_to_accel_addr(packed_weights));
    conv1d_write_reg(CONV1D_BIAS_BASE, ptr_to_accel_addr(prepared_bias));
    conv1d_write_reg(CONV1D_OUTPUT_BASE, ptr_to_accel_addr(output));
    conv1d_write_reg(CONV1D_INPUT_LEN, (uint32_t)input_len);
    conv1d_write_reg(CONV1D_IN_CH, (uint32_t)in_ch);
    conv1d_write_reg(CONV1D_OUT_CH, (uint32_t)out_ch);

    /*
     * NNoM Conv2D replacement keeps ReLU disabled here because generated
     * models place ReLU in a separate layer. The scalar shift field remains
     * programmed to zero while the per-output-channel table carries shifts.
     */
    conv1d_write_reg(CONV1D_QUANT, conv1d_make_quant_reg(0, 0));
    conv1d_load_output_shifts_u8(output_shifts, out_ch);

    conv1d_start();

    for (timeout = 0; timeout < CONV1D_ACCEL_TIMEOUT; timeout++) {
        if (conv1d_done())
            return 1;
    }

    g_conv1d_accel_stats.conv1d_hw_timeouts++;
    return 0;
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
