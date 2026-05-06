/*
 * kws_spike_repro.c — bit-exact reproduction of test/spikedebug/kws_spike_debug.c
 *                     for the SoC firmware (Hazard3 + Conv1D accelerator).
 *
 * Feeds the same 8000-sample int8 clip Spike used (clip 0 of
 * test/spikedebug/down_audio_8k_spike.bin, embedded as `spike_clip[]` by
 * scripts/spike_bin_to_c_array.py) and registers an NNoM per-layer callback
 * that dumps every output element exactly as kws_spike_debug.c's
 * `printf("%02x\n", ...)` does.
 *
 * Output goes to UART, LF-only (no CR), so a `tr -d '\r' | diff` against
 * sim/spike's activations.log isolates any layer where Hazard3 + accelerator
 * drift from Spike's pure-SW NNoM reference.
 *
 * Build:  make test-mel-compact-int8-peak-norm-dump
 * Run:    ./build/verilator/Vkws_soc --no-jtag \
 *             --flash test/build/mel_compact_int8_peak_norm_dump_xip_accel.bin \
 *             --cycles 800000000
 */

#include <stdint.h>
#include <string.h>

#include "nnom.h"          /* nnom_status_t, nnom_layer_t, default_layer_names */

#ifndef KWS_WEIGHTS_HEADER
#  error "KWS_WEIGHTS_HEADER must be passed via -D"
#endif
#include KWS_WEIGHTS_HEADER

#ifndef NNOM_STATIC_BUF_KB
#  error "NNOM_STATIC_BUF_KB must be passed via -D"
#endif

#define NUM_CLASSES       11
#define SAMPLES_PER_CLIP  8000

/* Audio clip generated from down_audio_8k_spike.bin (clip 0). */
extern const int8_t spike_clip[SAMPLES_PER_CLIP];

/* Class names — order MUST match the weights header / activations.log. */
static const char *const class_names[NUM_CLASSES] = {
    "down", "go", "left", "no", "off", "on",
    "right", "stop", "up", "yes", "unknown"
};

/* ── UART (LF-only — match Spike's stdio newline convention) ─────────────── */

#define UART_BASE            0x40004000UL
#define UART_REG(off)        (*(volatile uint32_t *)(UART_BASE + (off)))
#define UART_CSR_OFFS        0x00
#define UART_DIV_OFFS        0x04
#define UART_FSTAT_OFFS      0x08
#define UART_TX_OFFS         0x0C
#define UART_CSR_EN_MASK     (1u << 0)
#define UART_FSTAT_TXFULL    (1u << 8)

#ifdef CLK_MHZ
#define SYS_CLK_HZ   ((uint32_t)((CLK_MHZ) * 1000000UL))
#define UART_DIV_VAL (((2u * SYS_CLK_HZ) + (UART_BAUD_RATE / 2u)) / UART_BAUD_RATE)
#endif

static void uart_init(void) {
#ifdef UART_DIV_VAL
    UART_REG(UART_DIV_OFFS) = UART_DIV_VAL;
#endif
    UART_REG(UART_CSR_OFFS) |= UART_CSR_EN_MASK;
}

static void uart_putc(char c) {
    while (UART_REG(UART_FSTAT_OFFS) & UART_FSTAT_TXFULL);
    UART_REG(UART_TX_OFFS) = (uint32_t)(uint8_t)c;
}

static void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

static void uart_putu(uint32_t v) {
    char buf[12];
    int i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v > 0) { buf[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i > 0) uart_putc(buf[--i]);
}

static void uart_putd(int32_t v) {
    if (v < 0) { uart_putc('-'); uart_putu((uint32_t)(-v)); }
    else       { uart_putu((uint32_t)v); }
}

static void uart_puthex_byte(uint8_t b) {
    static const char hex[] = "0123456789abcdef";
    uart_putc(hex[(b >> 4) & 0xF]);
    uart_putc(hex[b & 0xF]);
}

/* ── NNoM static memory (sized via -DNNOM_STATIC_BUF_KB=N) ───────────────── */
#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t nnom_static_buf[NNOM_STATIC_BUF_KB * 1024];
#endif

/* ── Per-layer dump callback (mirrors kws_spike_debug.c::layer_dump_cb) ──── */

static int g_layer_idx = 0;

static nnom_status_t layer_dump_cb(nnom_model_t *m, nnom_layer_t *layer) {
    (void)m;
    nnom_tensor_t *t = layer->out->tensor;

    uint32_t n = 1;
    for (int d = 0; d < (int)t->num_dim; d++) n *= t->dim[d];

    uart_puts("LAYER_");        uart_putu((uint32_t)g_layer_idx);
    uart_puts(" type=");        uart_puts(default_layer_names[layer->type]);
    uart_puts(" shape=[");
    for (int d = 0; d < (int)t->num_dim; d++) {
        if (d) uart_putc(',');
        uart_putu((uint32_t)t->dim[d]);
    }
    uart_puts("] q_dec=");      uart_putd(t->q_dec ? (int32_t)t->q_dec[0] : -1);
    uart_puts(" n=");           uart_putu(n);
    uart_putc('\n');

    const uint8_t *p = (const uint8_t *)t->p_data;
    for (uint32_t i = 0; i < n; i++) {
        uart_puthex_byte(p[i]);
        uart_putc('\n');
    }

    g_layer_idx++;
    return NN_SUCCESS;
}

/* ── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    uart_init();

#ifdef NNOM_USING_STATIC_MEMORY
    nnom_set_static_buf(nnom_static_buf, sizeof(nnom_static_buf));
#endif

    nnom_model_t *model = nnom_model_create();
    if (!model) {
        uart_puts("ERROR: nnom_model_create failed\n");
        while (1);
    }
    if (model->head == NULL || model->head->in->tensor->p_data == NULL) {
        uart_puts("ERROR: model_compile failed — NNOM_STATIC_BUF_KB too small\n");
        while (1);
    }

    /* Feed Spike's clip 0 directly into the model input. */
    memcpy(nnom_input_data, spike_clip, SAMPLES_PER_CLIP);

    /* Register the per-layer dump and run inference once. */
    model_set_callback(model, layer_dump_cb);
    model_run(model);

    /* Final prediction line — matches Spike's "PRED:%d (%s)\n". */
    int    pred = 0;
    int8_t best = nnom_output_data[0];
    for (int j = 1; j < NUM_CLASSES; j++) {
        if (nnom_output_data[j] > best) { best = nnom_output_data[j]; pred = j; }
    }
    uart_puts("PRED:");
    uart_putu((uint32_t)pred);
    uart_puts(" (");
    uart_puts(class_names[pred]);
    uart_puts(")\n");

    while (1) asm volatile ("wfi");
    return 0;
}
