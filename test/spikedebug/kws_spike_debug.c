/*
 * kws_spike_debug.c — single-sample inference with per-layer activation dump
 *
 * INPUT:  down_audio_8k_spike.bin  (batched format: int32 n_clips,
 *         uint8 labels[128], int8 audio[128*8000] ...  clip 0 used)
 *
 * OUTPUT: stdout
 *   LAYER_<n> type=<name> shape=[d0,d1,...] q_dec=<val> n=<total_bytes>
 *   <hex byte>          <- one per line, all output elements of that layer
 *   ...
 *   PRED:<class_idx>
 *
 * Compare the per-layer hex dumps against hardware sim layer outputs.
 * q_dec tells you the Q-format fractional bits (value = int8 / 2^q_dec).
 *
 * Build & run:
 *   bash build.sh
 *   /opt/riscv/bin/spike --isa=rv32imac_zicsr_zifencei -m256 \
 *       /opt/riscv/riscv32-unknown-elf/bin/pk kws_spike_debug \
 *       | tee activations.log
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "weights_int8norm.h"

#ifndef NNOM_STATIC_BUF_SIZE
#define NNOM_STATIC_BUF_SIZE (512 * 1024)
#endif
#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t nnom_static_buf[NNOM_STATIC_BUF_SIZE];
#endif

#define NUM_CLASSES      11
#define SAMPLES_PER_CLIP 8000

static const char * const class_names[NUM_CLASSES] = {
    "down","go","left","no","off","on","right","stop","up","yes","unknown"
};

static int g_layer_idx = 0;

static nnom_status_t layer_dump_cb(nnom_model_t *m, nnom_layer_t *layer)
{
    (void)m;
    nnom_tensor_t *t = layer->out->tensor;

    /* total elements = product of all dims */
    uint32_t n = 1;
    for (int d = 0; d < (int)t->num_dim; d++)
        n *= t->dim[d];

    /* header line */
    printf("LAYER_%d type=%s shape=[", g_layer_idx,
           default_layer_names[layer->type]);
    for (int d = 0; d < (int)t->num_dim; d++) {
        if (d) printf(",");
        printf("%u", (unsigned)t->dim[d]);
    }
    printf("] q_dec=%d n=%u\n",
           t->q_dec ? (int)t->q_dec[0] : -1,
           (unsigned)n);

    /* raw hex dump — one byte per line for easy diff with hw sim */
    const uint8_t *p = (const uint8_t *)t->p_data;
    for (uint32_t i = 0; i < n; i++)
        printf("%02x\n", p[i]);

    g_layer_idx++;
    return NN_SUCCESS;
}

int main(void)
{
    /* ── load clip 0 from batched bin ──────────────────────────────── */
    FILE *fp = fopen("down_audio_8k_spike.bin", "rb");
    if (!fp) { printf("ERROR: cannot open down_audio_8k_spike.bin\n"); return 1; }

    /* skip int32 n_clips (4 bytes) + uint8 labels[128] (128 bytes) */
    fseek(fp, 4 + 128, SEEK_SET);
    int8_t audio[SAMPLES_PER_CLIP];
    if (fread(audio, 1, SAMPLES_PER_CLIP, fp) != SAMPLES_PER_CLIP) {
        printf("ERROR: short read on audio\n"); fclose(fp); return 1;
    }
    fclose(fp);

    /* ── init NNoM ─────────────────────────────────────────────────── */
#ifdef NNOM_USING_STATIC_MEMORY
    nnom_set_static_buf(nnom_static_buf, sizeof(nnom_static_buf));
#endif

    nnom_model_t *model = nnom_model_create();
    if (!model) { printf("ERROR: nnom_model_create() failed\n"); return 1; }

    /* ── copy input & register callback ───────────────────────────── */
    memcpy(nnom_input_data, audio, SAMPLES_PER_CLIP);
    model_set_callback(model, layer_dump_cb);

    /* ── run (callback fires after each layer while buffer is live) ── */
    model_run(model);

    /* ── print prediction ──────────────────────────────────────────── */
    int pred = 0;
    int8_t best = nnom_output_data[0];
    for (int j = 1; j < NUM_CLASSES; j++)
        if (nnom_output_data[j] > best) { best = nnom_output_data[j]; pred = j; }

    printf("PRED:%d (%s)\n", pred, class_names[pred]);

    model_delete(model);
    return 0;
}
