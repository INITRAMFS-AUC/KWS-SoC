/* nnom_conv1d_hw.c — drop-in replacement for nnom_conv2d.c with conv1d HW accel.
 *
 * This file replaces $(NNOM_DIR)/src/layers/nnom_conv2d.c in the accelerated
 * XIP build.  All non-run functions are identical to the original; conv2d_run
 * has a fast path that calls the KWS-SoC Conv1D hardware accelerator.
 *
 * Accelerated path requirements (all must hold at run time):
 *   - HWC layout (NNOM_USING_CHW not defined)
 *   - int8 input (bitwidth == 8)
 *   - H dimension == 1  (our 1-D conv embedded in a 2-D tensor)
 *   - No dilation (dilation.w == dilation.h == 1)
 *   - C_out <= ACCEL_MAX_CHOUT  (scratch buffer limit)
 *
 * For PADDING_SAME layers the input is zero-padded into a scratch buffer.
 *
 * Bias pre-processing exactly mirrors NNoM's formula:
 *   bias32[c] = ((int32)bias8[c] << bias_lshift[c]) + NNOM_ROUND(output_rshift[c])
 * where NNOM_ROUND(n) = (1<<n)>>1.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "nnom.h"
#include "nnom_local.h"
#include "nnom_layers.h"
#include "layers/nnom_conv2d.h"
#include "accel_conv1d.h"

#ifdef NNOM_USING_CMSIS_NN
#include "arm_math.h"
#include "arm_nnfunctions.h"
#endif

/* Maximum supported output channels (size of static scratch buffers). */
#define ACCEL_MAX_CHOUT     64

/* Maximum pre-padded input bytes.
 * Worst case: body layer 1 after front-end pool: (248+2)*36 = 9000 bytes. */
#define ACCEL_PAD_BUF_SIZE  9216

/* Static scratch buffers (safe for single-threaded MCU). */
static int32_t s_bias32[ACCEL_MAX_CHOUT];
static uint8_t s_shift_u8[ACCEL_MAX_CHOUT];
static int8_t  s_pad_buf[ACCEL_PAD_BUF_SIZE];

/* ---- All non-run functions copied verbatim from nnom_conv2d.c ---- */

nnom_layer_t *conv2d_s(const nnom_conv2d_config_t *config)
{
	nnom_conv2d_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;
	size_t mem_size;

	mem_size = sizeof(nnom_conv2d_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	in  = (void *)((uint8_t*)layer + sizeof(nnom_conv2d_layer_t));
	out = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	layer->super.type = NNOM_CONV_2D;
	in->type  = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	layer->super.in  = io_init(layer, in);
	layer->super.out = io_init(layer, out);
#ifdef NNOM_USING_CMSIS_NN
	layer->super.comp = comp;
#endif
	layer->super.run   = conv2d_run;
	layer->super.build = conv2d_build;
	layer->super.free  = conv2d_free;

	layer->super.config = (void*)config;

	if (config->weight->num_dim == 3) {
		layer->kernel   = kernel(1, config->kernel_size[0]);
		layer->stride   = stride(1, config->stride_size[0]);
		layer->dilation = dilation(1, config->dilation_size[0]);
	} else {
		layer->kernel   = kernel(config->kernel_size[0], config->kernel_size[1]);
		layer->stride   = stride(config->stride_size[0], config->stride_size[1]);
		layer->dilation = dilation(config->dilation_size[0], config->dilation_size[1]);
	}

	layer->filter_mult   = config->filter_size;
	layer->padding_type  = config->padding_type;
	layer->weight        = config->weight;
	layer->bias          = config->bias;
	layer->output_rshift = (nnom_qformat_param_t *)config->output_shift;
	layer->bias_lshift   = (nnom_qformat_param_t *)config->bias_shift;

	if (layer->padding_type == PADDING_SAME) {
		layer->pad.h = layer->dilation.h * (layer->kernel.h - 1) / 2;
		layer->pad.w = layer->dilation.w * (layer->kernel.w - 1) / 2;
		layer->pad.c = (1 - 1) / 2;
	}

	return (nnom_layer_t *)layer;
}

nnom_layer_t *Conv2D(uint32_t filters, nnom_3d_shape_t k, nnom_3d_shape_t s, nnom_3d_shape_t d,
                     nnom_padding_t pad_type, const nnom_weight_t *w, const nnom_bias_t *b)
{
	nnom_conv2d_layer_t *layer;
	nnom_buf_t *comp;
	nnom_layer_io_t *in, *out;
	size_t mem_size = sizeof(nnom_conv2d_layer_t) + sizeof(nnom_layer_io_t) * 2 + sizeof(nnom_buf_t);
	layer = nnom_mem(mem_size);
	if (layer == NULL)
		return NULL;

	in   = (void *)((uint8_t*)layer + sizeof(nnom_conv2d_layer_t));
	out  = (void *)((uint8_t*)in + sizeof(nnom_layer_io_t));
	comp = (void *)((uint8_t*)out + sizeof(nnom_layer_io_t));

	layer->super.type  = NNOM_CONV_2D;
	in->type  = NNOM_TENSOR_BUF_TEMP;
	out->type = NNOM_TENSOR_BUF_TEMP;
	comp->type = NNOM_TENSOR_BUF_TEMP;
	layer->super.in  = io_init(layer, in);
	layer->super.out = io_init(layer, out);
#ifdef NNOM_USING_CMSIS_NN
	layer->super.comp = comp;
#endif
	layer->super.run   = conv2d_run;
	layer->super.build = conv2d_build;

	layer->kernel       = k;
	layer->stride       = s;
	layer->dilation     = d;
	layer->filter_mult  = filters;
	layer->padding_type = pad_type;

	layer->weight = new_tensor(NNOM_QTYPE_PER_TENSOR, 4, filters);
	layer->bias   = new_tensor(NNOM_QTYPE_PER_TENSOR, 1, filters);

	{
		nnom_shape_data_t dim[4] = {k.h, k.w, k.c, filters};
		*(layer->weight->q_offset) = 0;
		*(layer->weight->q_dec)    = 0;
		layer->weight->p_data   = (void*)w->p_value;
		layer->weight->bitwidth = 8;
		layer->weight->qtype    = NNOM_QTYPE_PER_TENSOR;
		nnom_memcpy(layer->weight->dim, dim, layer->weight->num_dim * sizeof(nnom_shape_data_t));

		dim[0] = filters;
		*(layer->bias->q_offset) = 0;
		*(layer->bias->q_dec)    = 0;
		layer->bias->p_data   = (void*)b->p_value;
		layer->bias->bitwidth = 8;
		layer->weight->qtype  = NNOM_QTYPE_PER_TENSOR;
		nnom_memcpy(layer->bias->dim, dim, layer->bias->num_dim * sizeof(nnom_shape_data_t));

		layer->output_rshift = (nnom_qformat_param_t *)&w->shift;
		layer->bias_lshift   = (nnom_qformat_param_t *)&b->shift;
	}

	return (nnom_layer_t *)layer;
}

uint32_t conv_output_length(uint32_t input_length, uint32_t filter_size,
                            nnom_padding_t padding, uint32_t stride, uint32_t dilation)
{
	if (input_length == 0) return 0;
	uint32_t dilated_filter_size = (filter_size - 1) * dilation + 1;
	uint32_t output_length;
	if (padding == PADDING_SAME)
		output_length = input_length;
	else
		output_length = input_length - dilated_filter_size + 1;
	return (output_length + stride - 1) / stride;
}

nnom_status_t conv2d_build(nnom_layer_t *layer)
{
	nnom_conv2d_layer_t *cl = (nnom_conv2d_layer_t *)layer;

	layer->in->tensor = layer->in->hook.io->tensor;

	layer->out->tensor = new_tensor(NNOM_QTYPE_PER_TENSOR, layer->in->tensor->num_dim, cl->filter_mult);
	tensor_cpy_attr(layer->out->tensor, layer->in->tensor);

	layer->out->tensor->q_dec[0] = layer->in->tensor->q_dec[0] + cl->weight->q_dec[0] - cl->output_rshift[0];
	if (layer->actail)
		layer->out->tensor->q_dec[0] = act_get_dec_bit(layer->actail->type, layer->out->tensor->q_dec[0]);

	layer->out->tensor->dim[0] = conv_output_length(layer->in->tensor->dim[0], cl->kernel.h, cl->padding_type, cl->stride.h, cl->dilation.h);
	layer->out->tensor->dim[1] = conv_output_length(layer->in->tensor->dim[1], cl->kernel.w, cl->padding_type, cl->stride.w, cl->dilation.w);
	layer->out->tensor->dim[2] = cl->filter_mult;

	if (cl->padding_type == PADDING_SAME) {
		cl->pad.w = cl->dilation.w * (cl->kernel.w - 1) / 2;
		cl->pad.h = cl->dilation.h * (cl->kernel.h - 1) / 2;
		cl->pad.c = 0;
	}

#ifdef NNOM_USING_CMSIS_NN
	layer->comp->size = 2 * 2 * layer->in->tensor->dim[2] * cl->kernel.w * cl->kernel.h;
#endif
	layer->stat.macc = cl->kernel.w * cl->kernel.h * layer->in->tensor->dim[2] * tensor_size(layer->out->tensor);
	return NN_SUCCESS;
}

nnom_status_t conv2d_free(nnom_layer_t *layer)
{
	if (!layer->config) {
		nnom_conv2d_layer_t *cl = (nnom_conv2d_layer_t *)layer;
		delete_tensor(cl->weight);
		delete_tensor(cl->bias);
	}
	return NN_SUCCESS;
}

/* ---- Hardware-accelerated conv2d_run ---- */

nnom_status_t conv2d_run(nnom_layer_t *layer)
{
	nnom_conv2d_layer_t *cl = (nnom_conv2d_layer_t *)layer;

#ifdef NNOM_USING_CHW
	/* CHW path — not accelerated. */
	if (layer->in->tensor->bitwidth == 16)
		local_convolve_CHW_q15_nonsquare(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->weight->p_data, layer->out->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, cl->pad.w, cl->pad.h, cl->stride.w, cl->stride.h, cl->dilation.w, cl->dilation.h,
			cl->bias->p_data, cl->bias_lshift, cl->output_rshift, cl->weight->qtype,
			layer->out->tensor->p_data,
			layer->out->tensor->dim[1], layer->out->tensor->dim[0], NULL, NULL);
	else
		local_convolve_CHW_q7_nonsquare(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->weight->p_data, layer->out->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, cl->pad.w, cl->pad.h, cl->stride.w, cl->stride.h, cl->dilation.w, cl->dilation.h,
			cl->bias->p_data, cl->bias_lshift, cl->output_rshift, cl->weight->qtype,
			layer->out->tensor->p_data,
			layer->out->tensor->dim[1], layer->out->tensor->dim[0], NULL, NULL);
	return NN_SUCCESS;

#else  /* HWC format */

	/* ------------------------------------------------------------------ */
	/* Hardware accelerator fast path                                       */
	/* Conditions: int8, H==1, no dilation, C_out fits scratch buffer.     */
	/* ------------------------------------------------------------------ */
	if (layer->in->tensor->bitwidth == 8
	    && layer->in->tensor->dim[0] == 1
	    && cl->dilation.w == 1 && cl->dilation.h == 1
	    && layer->out->tensor->dim[2] <= ACCEL_MAX_CHOUT)
	{
		const uint8_t  c_in   = (uint8_t)layer->in->tensor->dim[2];
		const uint8_t  c_out  = (uint8_t)layer->out->tensor->dim[2];
		const uint8_t  k_w    = (uint8_t)cl->kernel.w;
		const uint8_t  stride = (uint8_t)cl->stride.w;
		const uint16_t w_in   = (uint16_t)layer->in->tensor->dim[1];

		const nnom_qformat_param_t *orshift = cl->output_rshift;
		const nnom_qformat_param_t *blshift = cl->bias_lshift;
		const int8_t               *bias8   = (const int8_t *)cl->bias->p_data;

		/* Build int32 bias (with pre-applied bias_lshift and NNOM_ROUND rounding)
		 * and uint8 per-channel output shift arrays. */
		for (uint8_t c = 0; c < c_out; c++) {
			int32_t rnd   = (1 << orshift[c]) >> 1;   /* NNOM_ROUND */
			s_bias32[c]   = ((int32_t)(int8_t)bias8[c] << blshift[c]) + rnd;
			s_shift_u8[c] = (uint8_t)orshift[c];
		}

		/* Handle padding: hardware does valid convolution only. */
		const int8_t *src;
		uint16_t      w_in_hw;

		if (cl->pad.w == 0) {
			src     = (const int8_t *)layer->in->tensor->p_data;
			w_in_hw = w_in;
		} else {
			const uint16_t pad    = (uint16_t)cl->pad.w;
			const uint16_t w_pad  = w_in + 2u * pad;
			const uint32_t left_b = (uint32_t)pad * c_in;
			const uint32_t mid_b  = (uint32_t)w_in * c_in;

			if ((uint32_t)w_pad * c_in > ACCEL_PAD_BUF_SIZE)
				goto sw_fallback;

			memset(s_pad_buf,           0, left_b);
			memcpy(s_pad_buf + left_b,
			       layer->in->tensor->p_data, mid_b);
			memset(s_pad_buf + left_b + mid_b, 0, (uint32_t)pad * c_in);

			src     = s_pad_buf;
			w_in_hw = w_pad;
		}

		accel_conv1d(
			src,
			(const int8_t *)cl->weight->p_data,
			(int8_t *)layer->out->tensor->p_data,
			s_bias32,
			s_shift_u8,
			c_in, c_out, k_w, stride, w_in_hw);

		return NN_SUCCESS;
	}

	/* ------------------------------------------------------------------ */
	/* Software fallback (16-bit, CHW forced off, large C_out, dilation)   */
	/* ------------------------------------------------------------------ */
sw_fallback:
	if (layer->in->tensor->bitwidth == 16)
		local_convolve_HWC_q15_nonsquare(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->weight->p_data, layer->out->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, cl->pad.w, cl->pad.h, cl->stride.w, cl->stride.h, cl->dilation.w, cl->dilation.h,
			cl->bias->p_data, cl->bias_lshift, cl->output_rshift, cl->weight->qtype,
			layer->out->tensor->p_data,
			layer->out->tensor->dim[1], layer->out->tensor->dim[0], NULL, NULL);
	else
		local_convolve_HWC_q7_nonsquare(
			layer->in->tensor->p_data,
			layer->in->tensor->dim[1], layer->in->tensor->dim[0], layer->in->tensor->dim[2],
			cl->weight->p_data, layer->out->tensor->dim[2],
			cl->kernel.w, cl->kernel.h, cl->pad.w, cl->pad.h, cl->stride.w, cl->stride.h, cl->dilation.w, cl->dilation.h,
			cl->bias->p_data, cl->bias_lshift, cl->output_rshift, cl->weight->qtype,
			layer->out->tensor->p_data,
			layer->out->tensor->dim[1], layer->out->tensor->dim[0], NULL, NULL);
	return NN_SUCCESS;

#endif /* NNOM_USING_CHW */
}
