/**
 * kws_bare_main.c — Shared bare-metal KWS inference firmware for KWS-SoC.
 *
 * One source file used by every NNoM int8 KWS variant (strided_s16_nodil,
 * mel_compact_4blk_ch36, …) so that cycle counts compare apples-to-apples.
 * The only thing the per-model build rule injects is the weights header:
 *
 *     -DKWS_WEIGHTS_HEADER='"strided_s16_nodil_weights.h"'
 *     -DKWS_MODEL_NAME='"strided_s16_nodil"'
 *     -DNNOM_STATIC_BUF_KB=52      (model-dependent activation peak)
 *
 * Optional:
 *     -DUSE_MCYCLE_CSR             enable in-firmware cycle measurement.
 *                                  Off by default — the Verilator/CXXRTL
 *                                  VPI testbench records the total sim
 *                                  cycle count too.  When on, prints per
 *                                  clip:
 *                                    CYCLES_CAPTURE: I2S+DMA+ISR window
 *                                                    (CPU mostly WFI but
 *                                                    SoC clock + perips
 *                                                    are alive — real
 *                                                    energy cost).
 *                                    CYCLES_INFER:   model_run() only.
 *                                    CYCLES_TOTAL:   capture + infer
 *                                                    (the budget that
 *                                                    matters for real-
 *                                                    time / power).
 *                                    CYCLES:         legacy alias =
 *                                                    CYCLES_INFER.
 *     -DKWS_DEBUG_DUMP_FIRST_CLIP  after the first full ring_buf is
 *                                  captured, dump every Q7 sample over
 *                                  UART in the same hex-word format as
 *                                  the i2s_mic_sim input file (bracketed
 *                                  by RB_BEGIN / RB_END), then ebreak.
 *                                  Diff against e.g. sim/down_0000.hex
 *                                  to expose any sample-level corruption
 *                                  in the I2S → DMA → ring_buf path.
 *
 * PIPELINE
 * --------
 *  i2s_irq (FIFO half-full) → DMAC PIRQ[0] (autonomous, no CPU)
 *  → DMA burst reads I2S_DMA_BURST_WORDS words into audio_ring
 *  → dmac_irq (IRQ 0) → ISR advances daddr, bumps bytes_written.
 *  Main waits via WFI until the ring holds a full second, snapshots
 *  it into nnom_input_data, runs inference, prints DETECT/CYCLES/IRQS.
 *
 *  CPU is not in the data path — only dmac_irq fires per burst, and the
 *  ISR is a 4-instruction stub.  i2s_irq is wired straight to the DMAC's
 *  PIRQ port (kws_soc.v), so each FIFO half-full self-arms the next burst
 *  with zero CPU latency, and FIFO drops are impossible at sample rates
 *  the AHB bus can absorb (always true at 8 kHz × 4-samples-per-word).
 *
 *  HW WIDTH=8 in apb_i2s_receiver packs LSB=oldest, so on this little-
 *  endian RV32 each 32-bit AHB write lands in time order — no
 *  byte-shuffle in the ISR, no end-of-clip ring_buf memcpy.  Saves
 *  ~58K cycles per clip and 8 KB of staging SRAM.  The weights
 *  header annotates nnom_input_data with aligned(4); the
 *  scripts/align_nnom_input.sh helper re-applies it on regen.
 *
 * REGISTER MAP (KWS-SoC)
 *  UART  0x40004000   uart_mini (115200 8N1, CLK_MHZ FPGA / sim)
 *  I2S   0x40008000   apb_i2s_receiver (INMP441 MEMS mic, 8 kHz mono int8)
 *  DMAC  0x60000000   MS_DMAC_AHBL (PIRQ-triggered on i2s_irq)
 *
 * CLASSES (index order must match the weights header):
 *  0=down 1=go 2=left 3=no 4=off 5=on 6=right 7=stop 8=up 9=yes 10=unknown
 */

#include <stdint.h>
#include <string.h>

#ifdef KWS_DUMP_LAYERS
/* Pulled in only for the spike-comparison dump build.  The dump callback
 * walks layer->out->tensor and uses default_layer_names[] from NNoM. */
#include "nnom.h"
#endif

#ifndef KWS_WEIGHTS_HEADER
#  error "KWS_WEIGHTS_HEADER must be passed via -D (e.g. '\"strided_s16_nodil_weights.h\"')"
#endif
#include KWS_WEIGHTS_HEADER

#ifndef KWS_MODEL_NAME
#  define KWS_MODEL_NAME "unknown"
#endif

/* ── UART (KWS-SoC uart_mini at 0x40004000) ─────────────────────────────── */

#define UART_BASE            0x40004000UL
#define UART_CSR_OFFS        0x00
#define UART_DIV_OFFS        0x04
#define UART_FSTAT_OFFS      0x08
#define UART_TX_OFFS         0x0C
#define UART_REG(off)        (*(volatile uint32_t *)(UART_BASE + (off)))
#define UART_CSR_EN_MASK     (1u << 0)
#define UART_FSTAT_TXFULL    (1u << 8)

/* CLK_MHZ and UART_BAUD_RATE injected by Makefile (-D flags). */
#ifdef CLK_MHZ
#define SYS_CLK_HZ   ((uint32_t)((CLK_MHZ) * 1000000UL))
#define UART_DIV_VAL (((2u * SYS_CLK_HZ) + (UART_BAUD_RATE / 2u)) / UART_BAUD_RATE)
#endif

static void uart_init(void) {
#ifdef UART_DIV_VAL
    UART_REG(UART_DIV_OFFS)  = UART_DIV_VAL;
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

static void uart_putdec(int v) {
    char buf[12];
    int i = 0;
    if (v < 0) { uart_putc('-'); v = -v; }
    if (v == 0) { uart_putc('0'); return; }
    while (v > 0) { buf[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i > 0) uart_putc(buf[--i]);
}

static void uart_puthex(uint32_t v) {
    static const char h[] = "0123456789abcdef";
    uart_putc('0'); uart_putc('x');
    for (int s = 28; s >= 0; s -= 4)
        uart_putc(h[(v >> s) & 0xfu]);
}

#ifdef KWS_DUMP_LAYERS
/* LF-only helpers for the spike-style activations dump.  Match
 * test/spikedebug/kws_spike_debug.c byte-for-byte: no '\r', lowercase hex,
 * "%02x"-style padding only on bytes (uint8). */
static void uart_putc_lf(char c)             { uart_putc(c); }
static void uart_puts_lf(const char *s)      { while (*s) uart_putc(*s++); }
static void uart_putu_lf(uint32_t v) {
    char b[12]; int i = 0;
    if (v == 0) { uart_putc('0'); return; }
    while (v > 0) { b[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i > 0) uart_putc(b[--i]);
}
static void uart_putd_lf(int32_t v) {
    if (v < 0) { uart_putc('-'); uart_putu_lf((uint32_t)(-v)); }
    else       { uart_putu_lf((uint32_t)v); }
}
static void uart_puthex_byte_lf(uint8_t b) {
    static const char h[] = "0123456789abcdef";
    uart_putc(h[(b >> 4) & 0xF]);
    uart_putc(h[b & 0xF]);
}

static int kws_layer_idx = 0;
static nnom_status_t kws_layer_dump_cb(nnom_model_t *m, nnom_layer_t *layer) {
    (void)m;
    nnom_tensor_t *t = layer->out->tensor;
    uint32_t n = 1;
    for (int d = 0; d < (int)t->num_dim; d++) n *= t->dim[d];

    uart_puts_lf("LAYER_");        uart_putu_lf((uint32_t)kws_layer_idx);
    uart_puts_lf(" type=");        uart_puts_lf(default_layer_names[layer->type]);
    uart_puts_lf(" shape=[");
    for (int d = 0; d < (int)t->num_dim; d++) {
        if (d) uart_putc_lf(',');
        uart_putu_lf((uint32_t)t->dim[d]);
    }
    uart_puts_lf("] q_dec=");
    uart_putd_lf(t->q_dec ? (int32_t)t->q_dec[0] : -1);
    uart_puts_lf(" n=");           uart_putu_lf(n);
    uart_putc_lf('\n');

    const uint8_t *p = (const uint8_t *)t->p_data;
    for (uint32_t i = 0; i < n; i++) {
        uart_puthex_byte_lf(p[i]);
        uart_putc_lf('\n');
    }
    kws_layer_idx++;
    return NN_SUCCESS;
}
#endif

/* ── I2S (KWS-SoC apb_i2s_receiver at 0x40008000) ──────────────────────── */

typedef struct {
    volatile uint32_t id;    /* 0x00: peripheral ID (ROV = 0xDEADCAFE)        */
    volatile uint32_t conf;  /* 0x04: [31:8] cfg_div
                              *       [7:6]  cfg_width  (00=32b, 01=16b, 10=8b)
                              *       [5]    cfg_ds_en  (HW 3× decimate)
                              *       [4]    cfg_irq_en                       */
    volatile uint32_t fifo;  /* 0x08: audio FIFO; with WIDTH=8 each 32-bit
                              *       read returns 4 packed int8 samples
                              *       (byte[0]=oldest, byte[3]=newest).       */
} i2s_hw_t;

#define I2S_BASE_ADDR    0x40008000UL
#define I2S_FIFO_PA      0x40008008UL   /* physical address of I2S->fifo for DMA */
#define I2S              ((i2s_hw_t *)I2S_BASE_ADDR)

/* DMA burst size in 32-bit words.  MUST equal the I2S receiver's half-
 * full IRQ threshold (= hardware FIFO_DEPTH / 2) or every other 4-block
 * in ring_buf is silently zero-padded by FIFO-empty reads.  Don't set
 * this directly — it's derived in test/Makefile from the root Makefile's
 * I2S_FIFO_DEPTH so the two can't drift apart.  Goes away once task #12
 * (autonomous DMA drain) lands. */
#ifndef I2S_DMA_BURST_WORDS
#  error "I2S_DMA_BURST_WORDS must be passed via -D (= I2S_FIFO_DEPTH / 2)"
#endif

/* I2S clock divisor.  apb_i2s_receiver toggles SCK every (cfg_div + 1)
 * system cycles.  One stereo frame is 64 SCK ticks (L + R), MONO_MODE keeps
 * only L, so the raw mic sample rate is:
 *
 *   raw_rate = clk / (128 * (cfg_div + 1))
 *
 *   36 MHz FPGA, ~16 kHz raw → cfg_div = 17  (15.625 kHz, SCK = 1 MHz)
 *   12 MHz sim,  ~16 kHz raw → cfg_div = 5   (15.625 kHz)
 *
 * The HW 3× decimator (cfg_ds_en) is OFF — we run the receiver at the
 * model's 16 kHz ring-rate directly, then the snapshot loop in main()
 * does a single ÷2 into nnom_input_data for the 8 kHz model input.
 * SCK at 1 MHz keeps the INMP441 mic out of low-power mode (its
 * datasheet wants ≥1 MHz to leave sleep), unlike the 500 kHz cfg_div=35
 * route.
 *
 * Override on the make line (e.g. -DI2S_CLK_DIV=5 for 12 MHz sim). */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  17
#endif

/* I2S_CONF bit masks — match peris/i2s/i2s_apb/i2s_itr2/i2s_regs.h.
 * WIDTH[7:6]: 0=32-bit (1 sample/word), 1=16-bit (2/word), 2=8-bit (4/word). */
#define I2S_CONF_IRQ_EN     (1u << 4)
#define I2S_CONF_DS_EN      (1u << 5)   /* HW 3x downsample (keep 1 of 3)    */
#define I2S_CONF_WIDTH_8    (2u << 6)   /* 4 int8 samples per FIFO word      */

/* HW decimator gate (default ON).
 *
 * Default 1: receiver does the ÷2 in hardware (i2s_rx_core drops every
 *            other captured frame).  At cfg_div=17 (CLK_MHZ=36) the raw
 *            frame rate is 15.625 kHz, so the post-HW-DS ring rate is
 *            ~7.81 kHz — within ±2.5 % of the model's nominal 8 kHz
 *            input.  Halves the bytes per second the DMA carries,
 *            halves the PIRQ rate, and lets KWS_DECIMATE collapse to 1
 *            (the snapshot loop becomes a contiguous memcpy instead of
 *            a strided gather).
 *            RING_HZ tracks this automatically below.
 *
 * Set to 0: receiver runs raw and main() does the ÷2 in firmware.
 *           Use when mic + receiver are wired so HW ÷2 would put the
 *           model below 7 kHz, or when bypassing HW is required for
 *           experimental work.  Output rate at the model is identical
 *           on both paths at cfg_div=17 — only the DMA traffic differs. */
#ifndef KWS_DS_EN
#define KWS_DS_EN 1
#endif

static void i2s_init(uint32_t clk_div) {
    /* WIDTH=8: HW packs 4 int8 samples per FIFO word — 4× less FIFO/AHB
     *          traffic per second of audio.  Receiver writes
     *          {sample4, sample3, sample2, sample1} into the 32-bit word
     *          (newest at MSB, oldest at LSB), so on little-endian RV32 the
     *          bytes land in audio_ring[] in time order — no firmware
     *          byte-shuffle needed.
     * DS_EN:   compile-time gate (KWS_DS_EN above).  Default 1 — the
     *          HW receiver does the ÷2; the firmware snapshot loop runs
     *          1:1 with the ring (KWS_DECIMATE collapses to 1).  Set
     *          to 0 to disable HW DS and rely on the firmware ÷2. */
    I2S->conf = ((clk_div & 0xFFFFFFu) << 8)
              | I2S_CONF_IRQ_EN
#if KWS_DS_EN
              | I2S_CONF_DS_EN
#endif
              | I2S_CONF_WIDTH_8;
}

/* ── MS_DMAC_AHBL (0x6000_0000) ─────────────────────────────────────────── */

typedef struct {
    volatile uint32_t control;
    volatile uint32_t status;
    volatile uint32_t saddr;
    volatile uint32_t daddr;
    volatile uint32_t count;
    volatile uint32_t swtrig;
    volatile uint32_t fcount;
    volatile uint32_t icr;
} dmac_hw_t;

#define DMAC_BASE  0x60000000UL
#define DMAC       ((dmac_hw_t *)DMAC_BASE)

/* PIRQ-triggered word transfer: fixed source (FIFO), incrementing dest.
 * The receiver's i2s_irq is wired to DMAC.PIRQ[0] in kws_soc.v, so each
 * FIFO half-full self-arms the next burst with no CPU latency.
 *   bit 0      EN=1
 *   bits 11:8  TRIGGER = 0001 (fire on PIRQ[0] = i2s_irq)
 *   bits 17:16 STYPE   = 2 (word)
 *   bits 20:18 SAI     = 0 (no source increment — FIFO)
 *   bits 25:24 DTYPE   = 2 (word)
 *   bits 28:26 DAI     = 4 (+4B per word) */
#define DMAC_CTRL_I2S_PIRQ \
    ((1u <<  0) | (1u <<  8) | (2u << 16) | (0u << 18) | (2u << 24) | (4u << 26))

#define DMA_BURST_BYTES        (I2S_DMA_BURST_WORDS * 4)
/* Audio rates: receiver delivers ~16 kHz int8 to audio_ring (3× HW decimate
 * from ~48 kHz mic); main loop decimates ÷2 again into the 8 kHz int8 model
 * input.  Keep these as named constants so the snapshot math reads clean. */
/* Audio rate parameterization.
 *
 * RING_HZ        Receiver-output rate (after any HW DS_EN).  Fixed by the
 *                I2S pipeline: 16 kHz today (16 kHz raw, no HW DS, OR
 *                48 kHz raw + HW ÷3).
 * KWS_MODEL_HZ   Rate the model expects.  Default 8 kHz (1-second clip
 *                = MODEL_SAMPLES_PER_CLIP=8000 int8 samples).  16 kHz
 *                models (peak-norm variant) override with -DKWS_MODEL_HZ=16000.
 * KWS_DECIMATE   Snapshot-loop stride (RING_HZ / KWS_MODEL_HZ).  1 for
 *                16 kHz models, 2 for 8 kHz models (firmware ÷2). */
/* RING_HZ tracks KWS_DS_EN.  When the HW receiver runs ÷2 (KWS_DS_EN=1)
 * the byte rate into audio_ring is half the raw I2S frame rate, so the
 * firmware-side ring rate must follow — otherwise KWS_DECIMATE doubles
 * up on the HW divide and the model gets 4 kHz garbage instead of 8 kHz
 * audio.  Override with -DKWS_RING_HZ=<n> if the receiver runs at a
 * non-standard divider. */
#ifndef KWS_RING_HZ
#  if KWS_DS_EN
#    define KWS_RING_HZ        8000
#  else
#    define KWS_RING_HZ        16000
#  endif
#endif
#define RING_HZ                KWS_RING_HZ
#ifndef KWS_MODEL_HZ
#define KWS_MODEL_HZ           8000
#endif
#define KWS_DECIMATE           (RING_HZ / KWS_MODEL_HZ)   /* 1 or 2 */
#define MODEL_SAMPLES_PER_CLIP 8000
#define RING_SAMPLES_PER_CLIP  (MODEL_SAMPLES_PER_CLIP * KWS_DECIMATE)
#define NUM_CLASSES            11

/* Audio capture ring buffer.
 *
 * The DMA writes packed Q8 samples (LSB=oldest in each word, see the
 * apb_i2s_receiver q8-lsb-oldest patch) continuously into this ring; on
 * each PIRQ the ISR just advances the DMA daddr by one burst, wrapping
 * to the start when it reaches the end.  The model never reads from this
 * buffer directly — main loop snapshots a 1-second window into
 * nnom_input_data, so we have no read-during-write race with model_run.
 *
 * Why a separate buffer (not nnom_input_data):
 *   1. While model_run reads input, the next clip's DMA must still be
 *      filling somewhere — pointing the DMA at nnom_input_data corrupts
 *      the very inference that's reading it.
 *   2. A future sliding-window setup (post Conv1D accelerator) needs the
 *      DMA to keep streaming continuously while inference processes
 *      overlapping windows — that requires a wrap-around ring distinct
 *      from the model's expected input layout.
 *
 * Sizing.  KWS_RING_SAMPLES is a power of 2 (cheap modular wrap) and
 * must be >= SAMPLES_PER_CLIP plus enough headroom that the DMA can't
 * overtake an in-progress memcpy.  At 16 kHz the memcpy of 16000 bytes
 * takes ~48K cycles (byte loop), during which the DMA writes ~10 bytes —
 * 384 bytes (16384 - 16000) is sufficient headroom. */
#ifndef KWS_RING_SAMPLES
#define KWS_RING_SAMPLES 16384
#endif
#define KWS_RING_MASK (KWS_RING_SAMPLES - 1)

/* Sliding-window step config.
 *
 * Each iteration of the inference loop computes:
 *
 *     step = clamp(max(KWS_MIN_STEP_RING_BYTES, inference_cycles / cycles_per_ring_byte),
 *                  1, RING_SAMPLES_PER_CLIP)
 *     bytes_at_next_inference += step
 *     snap_start = bytes_at_next_inference - RING_SAMPLES_PER_CLIP
 *
 * Properties:
 *   - 20 ms minimum cadence (KWS_MIN_STEP_RING_BYTES = 20 ms × ring_rate).
 *     Faster inference doesn't slide faster than 20 ms — gives the CPU
 *     idle time between bursts when inference is unusually quick.
 *   - 1-clip maximum step.  Clamping at RING_SAMPLES_PER_CLIP guarantees
 *     that consecutive windows are contiguous (no gap), so every audio
 *     byte is part of at least one window.
 *   - When inference is slower than RING_SAMPLES_PER_CLIP / ring_rate
 *     (≈ 1 s), the firmware can't keep up — bytes_written runs ahead
 *     of bytes_at_next_inference faster than it can be processed.  The
 *     loop detects this (gap > RING_SAMPLES_PER_CLIP), prints an
 *     AUDIO_LOSS warning, and resyncs bytes_at_next_inference to the
 *     latest possible window.  Audio between the old and new windows
 *     is forfeit.  Only matters for SW-only model paths today.
 *
 * Override either floor with -DKWS_MIN_STEP_RING_BYTES=N or pin to a
 * fixed step with -DKWS_STEP_SAMPLES=N (legacy knob; disables the
 * dynamic computation entirely). */
#ifndef KWS_MIN_STEP_RING_BYTES
#define KWS_MIN_STEP_RING_BYTES (RING_HZ / 50)   /* 20 ms at 16 kHz = 320 */
#endif

#ifdef CLK_MHZ
#define CYCLES_PER_RING_BYTE ((uint32_t)((CLK_MHZ) * 1000000UL) / RING_HZ)
#else
#define CYCLES_PER_RING_BYTE 2250u  /* 36 MHz / 16 kHz; matches the FPGA build */
#endif

__attribute__((aligned(4))) static volatile int8_t audio_ring[KWS_RING_SAMPLES];

/* Per-clip peak normalization — applied to nnom_input_data[] after snapshot.
 * Matches the Python training pipeline exactly:
 *   scale = min(96 / peak, 64)   where peak = max(|s|) over the clip
 *   out[i] = clip(round(s * scale), -128, 127)
 * Integer arithmetic only. When peak==1 the ×64 cap applies.
 * Silence (peak==0) is left untouched.                                       */
static void peak_norm_clip(int8_t *buf, int n)
{
    int32_t peak = 0;
    for (int i = 0; i < n; i++) {
        int32_t a = buf[i] < 0 ? -(int32_t)buf[i] : (int32_t)buf[i];
        if (a > peak) peak = a;
    }
    if (peak < 1) return;
    for (int i = 0; i < n; i++) {
        int32_t s = buf[i];
        int32_t scaled;
        if (peak == 1) {
            scaled = s * 64;
        } else {
            int32_t half = peak >> 1;
            scaled = s >= 0 ? (s * 96 + half) / peak
                            : (s * 96 - half) / peak;
        }
        if (scaled >  127) scaled =  127;
        if (scaled < -128) scaled = -128;
        buf[i] = (int8_t)scaled;
    }
}

static void dma_arm(void) {
    DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u;        /* EN=0 while reconfiguring */
    DMAC->saddr   = I2S_FIFO_PA;
    DMAC->daddr   = (uint32_t)(uintptr_t)audio_ring;
    DMAC->count   = (uint32_t)(I2S_DMA_BURST_WORDS - 1); /* COUNT = N-1 for N transfers */
    DMAC->icr     = 1u;                              /* clear any leftover sticky IRQ */
    DMAC->control = DMAC_CTRL_I2S_PIRQ;              /* EN=1 — autonomous PIRQ-armed */
}

/* ── RISC-V CSR helpers ──────────────────────────────────────────────────── */

static inline void csr_set_mtvec(void (*handler)(void)) {
    asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)handler));
}
/* Hazard3 inhibits mcycle out of reset (mcountinhibit.cy = 1).  The
 * dynamic sliding-window step in main() reads mcycle for *correctness*,
 * not just debug — every build needs the counter ungated.  Keep this
 * outside the USE_MCYCLE_CSR gate (which now controls only the UART
 * cycle-print lines, not whether mcycle ticks). */
static inline void csr_enable_cycle_counter(void) {
    asm volatile ("csrw 0x320, zero");
}
static inline void csr_enable_meie(void) {
    asm volatile ("csrs mie, %0" :: "r"(1u << 11));
}
static inline void csr_enable_mie(void) {
    asm volatile ("csrsi mstatus, 8");
}
static inline void csr_meiea_kws_en(void) {
    /* Unmask only dmac_irq (IRQ 0, bit 16) in Hazard3 MEIEA group 0.
     * i2s_irq is consumed by DMAC.PIRQ[0] in HW, not by the CPU, so it
     * stays masked — keeping the CPU out of the per-burst data path.
     * kws_soc.v: .irq({i2s_irq, uart_irq, dmac_irq}) — IRQ 0 = dmac_irq. */
    asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16));
}

/* ── NNoM static memory buffer ───────────────────────────────────────────── */
/* Each model has its own peak activation size — must be set per build via
 *   -DNNOM_STATIC_BUF_KB=<kb>
 * in the model's CFLAGS (see test/Makefile). No default: an undersized
 * buffer would silently fail model_compile and crash on the first run. */
#ifndef NNOM_STATIC_BUF_KB
#  error "NNOM_STATIC_BUF_KB must be passed via -D (model peak activation, in KB)"
#endif

#ifndef KWS_DEBOUNCE_COUNT
#define KWS_DEBOUNCE_COUNT 2
#endif

/* Confidence threshold for a single inference clip.  Clips whose argmax
 * int8 softmax score is below this are excluded from the soft-vote window.
 * 60/128 ≈ 47% — high enough to reject background noise while still
 * accepting genuine word predictions.  Override with -DKWS_CONF_THRESH=N. */
#ifndef KWS_CONF_THRESH
#define KWS_CONF_THRESH 60
#endif

/* Minimum number of confident clips required in a KWS_PRINT_INTERVAL_MS
 * window before a DETECT line is emitted.  Prevents a single lucky
 * noise clip from triggering a false positive.
 * Override with -DKWS_MIN_VOTE_CLIPS=N. */
#ifndef KWS_MIN_VOTE_CLIPS
#define KWS_MIN_VOTE_CLIPS 3
#endif

/* UART output rate gate.  With the dynamic sliding window, inference can
 * happen 10–50× per second; printing once per iter floods the UART and
 * extends each iter's wall time enough to feed back into AUDIO_LOSS.
 * Instead, emit a single DETECT line at most every KWS_PRINT_INTERVAL_MS
 * milliseconds — the wall time gating is anchored to bytes_written
 * (DMA-side, hardware truth) rather than mcycle, so the rate is stable
 * regardless of how fast inference runs.  Set =0 to disable the gate
 * (every iter prints, like older builds). */
#ifndef KWS_PRINT_INTERVAL_MS
#define KWS_PRINT_INTERVAL_MS 200
#endif
#define KWS_PRINT_INTERVAL_RING_BYTES \
    (((uint32_t)RING_HZ * (uint32_t)KWS_PRINT_INTERVAL_MS) / 1000u)

#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t nnom_static_buf[NNOM_STATIC_BUF_KB * 1024];
#endif

/* ── Audio capture state ─────────────────────────────────────────────────── */
/* `bytes_written` is a 32-bit monotonic counter of bytes the DMA has
 * deposited into audio_ring.  At 16 kHz it wraps every ~3 days, plenty for
 * any realistic deployment.  Main loop compares against
 * `bytes_at_next_inference` to decide when the next 1-second window is
 * available; the comparison uses unsigned subtraction so it handles the
 * eventual 32-bit wrap correctly as long as the gap stays bounded. */
static volatile uint32_t bytes_written = 0;
static volatile int      i2s_irq_count = 0;

/* ── Class names (order must match the weights header) ───────────────────── */
static const char * const class_names[NUM_CLASSES] = {
    "down", "go", "left", "no", "off", "on",
    "right", "stop", "up", "yes", "unknown"
};

/* ── Machine trap handler ────────────────────────────────────────────────── */
/*
 *  Only dmac_irq (IRQ 0, MEIEA bit 16) reaches the CPU.  i2s_irq drives
 *  the DMA's PIRQ port directly (kws_soc.v), so CPU only sees burst-done
 *  events.  The handler:
 *    1. Clears the DMAC's sticky ICR latch.
 *    2. Advances DADDR by one burst (wrapping at end of audio_ring) so the
 *       next PIRQ-triggered burst writes into the next slot.
 *    3. Bumps bytes_written so main can detect a complete clip.
 *    4. Read-back of DMAC->control is an AHB fence ensuring ICR clear
 *       lands before mret.
 *  Exceptions (mcause MSB clear) print debug info and halt.
 */
void __attribute__((interrupt("machine"), aligned(4))) kws_trap_handler(void) {
    uint32_t mcause;
    asm volatile ("csrr %0, mcause" : "=r"(mcause));

    if (!(mcause & 0x80000000u)) {
        uint32_t mepc, mtval;
        asm volatile ("csrr %0, mepc"  : "=r"(mepc));
        asm volatile ("csrr %0, mtval" : "=r"(mtval));
        uart_puts("EXCEPTION cause=");
        uart_putdec((int)(mcause & 0x7fffffffu));
        uart_puts(" mepc=");
        uart_puthex(mepc);
        uart_puts(" mtval=");
        uart_puthex(mtval);
        uart_puts("\r\n");
        while (1);
    }

    DMAC->icr = 1u;

    uint32_t next_daddr = DMAC->daddr + DMA_BURST_BYTES;
    if (next_daddr >= (uint32_t)(uintptr_t)audio_ring + KWS_RING_SAMPLES)
        next_daddr = (uint32_t)(uintptr_t)audio_ring;
    DMAC->daddr = next_daddr;

    bytes_written += DMA_BURST_BYTES;
    i2s_irq_count++;

    (void)DMAC->control;   /* AHB read-back fence */
}

/* ── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    uart_init();
    uart_puts("KWS bare-metal firmware (" KWS_MODEL_NAME ", NNoM int8)\r\n");

#ifdef NNOM_USING_STATIC_MEMORY
    nnom_set_static_buf(nnom_static_buf, sizeof(nnom_static_buf));
#endif

    nnom_model_t *model = nnom_model_create();
    if (!model) {
        uart_puts("ERROR: nnom_model_create failed\r\n");
        while (1);
    }
    if (model->head == NULL || model->head->in->tensor->p_data == NULL) {
        uart_puts("ERROR: model_compile failed — nnom_static_buf too small\r\n");
        while (1);
    }
    uart_puts("Model loaded\r\n");

#ifdef KWS_DUMP_LAYERS
    /* Single-shot mode for diff-against-Spike: register the per-layer
     * activation dump callback (defined at file scope) before the first
     * model_run.  Output format mirrors test/spikedebug/kws_spike_debug.c
     * byte-for-byte (LF-only, "%02x"-padded bytes). */
    model_set_callback(model, kws_layer_dump_cb);
#endif

    csr_enable_cycle_counter();
    csr_set_mtvec(kws_trap_handler);
    csr_enable_meie();
    csr_meiea_kws_en();   /* enable dmac_irq (IRQ 0) + i2s_irq (IRQ 2) */
    dma_arm();
    i2s_init(I2S_CLK_DIV);
    uart_puts("I2S started\r\n");
    csr_enable_mie();

#ifdef KWS_DEBUG_DUMP_FIRST_CLIP
    int dumped = 0;
#endif

    /* `bytes_at_next_inference` is the bytes_written threshold that
     * marks the END of the next snapshot's window.  Initialised to one
     * full clip (so iteration 0 waits for the first 1 s of audio); each
     * subsequent iteration advances it by the dynamic `step_bytes`
     * computed at the bottom of the loop.  Snapshots are anchored to
     * this counter, NOT to bytes_written, so the window position tracks
     * the explicit step regardless of how far the DMA has run ahead. */
    uint32_t bytes_at_next_inference = RING_SAMPLES_PER_CLIP;
    uint32_t step_bytes              = RING_SAMPLES_PER_CLIP; /* iter-0 default */
    int last_pred    = NUM_CLASSES - 1;
    int debounce_cnt = 0;

#ifdef USE_MCYCLE_CSR
    /* Capture-window measurement: read mcycle when we *start* waiting
     * for the next 1 s of audio, then again when bytes_written reaches
     * the threshold.  Difference is "cycles spent waiting for capture
     * to catch up to the next inference window".  For the first clip
     * this matches the audio rate exactly (~clk_hz cycles, no DMA
     * overhead); for subsequent clips it usually returns ~0 because
     * the ring kept filling during model_run and the threshold has
     * already been crossed. */
    uint32_t cyc_capture_start;
    asm volatile ("csrr %0, mcycle" : "=r"(cyc_capture_start));
#endif

    while (1) {
        /* Wait until bytes_written reaches the END of the next window. */
        while ((int32_t)(bytes_written - bytes_at_next_inference) < 0) {
            asm volatile ("wfi");
        }

        /* Iteration ground-truth start: snapshot bytes_written so we can
         * compute step_bytes at the end of the loop as the *actual* number
         * of ring bytes the DMA wrote during this iteration.  Using the
         * DMA counter directly is more robust than deriving step from
         * mcycle: mcycle/CYCLES_PER_RING_BYTE depends on the assumed
         * ring rate matching the I2S divider exactly, and (observed in
         * sim) mcycle does not always tick 1:1 with the DMA's wall-time
         * fill rate — there's a several-MHz drift per iteration that
         * was enough to trip the audio-loss guard within ~30 s, even
         * though the ring was never actually overwritten.  bytes_written
         * is incremented by exactly DMA_BURST_BYTES on every PIRQ, so
         * the delta is a hardware-anchored measurement of how many
         * bytes the DMA wrote between iter_start and iter_end. */
        uint32_t iter_start_bw = bytes_written;

        /* Audio-loss guard.  If bytes_written has run more than one
         * RING_SAMPLES_PER_CLIP ahead of bytes_at_next_inference, the
         * inference can't keep up and the ring has overwritten bytes
         * we still need.  Snap the threshold forward to the latest
         * possible window and shout about it.  Subsequent windows will
         * pick up from there. */
        if ((int32_t)(bytes_written - bytes_at_next_inference) > (int32_t)RING_SAMPLES_PER_CLIP) {
            uint32_t lost = (bytes_written - bytes_at_next_inference) - RING_SAMPLES_PER_CLIP;
            uart_puts("AUDIO_LOSS bytes=");
            uart_putdec((int)lost);
            uart_puts("\r\n");
            bytes_at_next_inference = bytes_written;  /* resync to "latest 16000 bytes" */
        }

#ifdef USE_MCYCLE_CSR
        uint32_t cyc_capture_end;
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_capture_end));
        uint32_t cycles_capture = cyc_capture_end - cyc_capture_start;
#endif

        /* Snapshot the next window — anchored to bytes_at_next_inference,
         * NOT to the floating bytes_written, so the window position
         * tracks the explicit step.  Decimate ÷KWS_DECIMATE into
         * nnom_input_data (=2 for 8 kHz models, =1 for 16 kHz models).
         * Optional AGC normalization is applied per sample (no-op when
         * KWS_AGC_ENABLE=0, the default). */
        uint32_t snap_start_byte = bytes_at_next_inference - RING_SAMPLES_PER_CLIP;
        uint32_t snap_start_ring = snap_start_byte & KWS_RING_MASK;
        for (int j = 0; j < MODEL_SAMPLES_PER_CLIP; j++) {
            uint32_t ring_idx =
                (snap_start_ring + (uint32_t)(j * KWS_DECIMATE)) & KWS_RING_MASK;
            nnom_input_data[j] = (int8_t)audio_ring[ring_idx];
        }
        peak_norm_clip(nnom_input_data, MODEL_SAMPLES_PER_CLIP);

#ifdef KWS_DEBUG_DUMP_FIRST_CLIP
        /* Dump the FIRST captured window over UART in the same hex-word
         * format as i2s_mic_sim's input file (q7 << 16, sign-extended in
         * the upper 16 bits) so the output diffs directly against e.g.
         * sim/down_0000.hex.  Bracketed by RB_BEGIN / RB_END for easy
         * `awk '/RB_BEGIN/,/RB_END/'` extraction.  No GDB needed —
         * decouples the diagnostic from LTO / DWARF / OpenOCD entirely. */
        if (!dumped) {
            dumped = 1;
            uart_puts("RB_BEGIN\r\n");
            for (int k = 0; k < MODEL_SAMPLES_PER_CLIP; k++) {
                uint32_t word = ((uint32_t)(int32_t)nnom_input_data[k] << 16)
                                & 0xffff0000u;
                uart_puthex(word);
                uart_puts("\r\n");
            }
            uart_puts("RB_END\r\n");
            asm volatile ("ebreak");
        }
#endif

#ifdef KWS_DUMP_LAYERS
        /* Single-shot dump: quiesce the audio path before inference.
         * The Conv1D accelerator drives heavy AHB traffic (writes
         * activations to SRAM, reads weights from XIP); concurrent DMA
         * bursts from the I2S receiver collide with it and stall
         * inference badly (observed: stuck mid-LAYER_8 for minutes in
         * sim).  Disable the mic, EN=0 the DMA, mask the dmac_irq
         * before model_run; halt after the dump. */
        I2S->conf      = 0;        /* stop SCK/WS — halts the FIFO push */
        DMAC->control &= ~1u;      /* DMA EN=0 — drop any pending PIRQ  */
        asm volatile ("csrci mstatus, 8");  /* global IRQs off          */
#endif

        /* Inference-only cycle count, kept around for the legacy
         * CYCLES_INFER UART line.  The dynamic-step computation no
         * longer reads this — it uses the full iteration wall time
         * captured below (see iter_start_cyc). */
        uint32_t step_cyc_start, step_cyc_end;
        asm volatile ("csrr %0, mcycle" : "=r"(step_cyc_start));
        model_run(model);
        asm volatile ("csrr %0, mcycle" : "=r"(step_cyc_end));
#ifdef USE_MCYCLE_CSR
        uint32_t cycles = step_cyc_end - step_cyc_start;
#endif

        int    pred      = 0;
        int8_t max_score = nnom_output_data[0];
        for (int j = 1; j < NUM_CLASSES; j++) {
            if (nnom_output_data[j] > max_score) {
                max_score = nnom_output_data[j];
                pred      = j;
            }
        }

        /* Confidence threshold: ~47% softmax (60/128). Skipped in dump
         * mode so the PRED line stays comparable to the Spike harness. */
#ifndef KWS_DUMP_LAYERS
        if (max_score < KWS_CONF_THRESH) pred = NUM_CLASSES - 1;
#endif

        if (pred == last_pred) {
            debounce_cnt++;
        } else {
            last_pred    = pred;
            debounce_cnt = 1;
        }
        int debounced = (debounce_cnt >= KWS_DEBOUNCE_COUNT) ? pred : (NUM_CLASSES - 1);

        /* Per-clip print gating.
         *   KWS_DUMP_LAYERS: suppress everything except the layer dump
         *                    + final PRED line (matches Spike harness).
         *   KWS_QUIET:       only print clips that resolved to a *real*
         *                    word (not "unknown" — the last class).  Cuts
         *                    the per-clip line storm on long captures of
         *                    mostly silence; reduces UART overhead and
         *                    helps the AUDIO_LOSS guard stay quiet too.
         *   default:         print every iteration. */

        /* Wall-clock-anchored output gate with score-averaging.
         *
         * The dynamic sliding window may run inference 10–50× per
         * second.  Printing once per iter floods the UART AND latches
         * onto whichever single inference happened to fire at the
         * print boundary.  Instead, we accumulate the int8 softmax
         * scores across every inference inside one
         * KWS_PRINT_INTERVAL_MS window and, at the boundary, emit the
         * argmax of the accumulated score (i.e. the soft-vote winner).
         * Voting on summed soft scores is more robust than voting on
         * argmax-per-clip (a noisy clip that's barely "yes" doesn't
         * out-vote three confident "no"s).
         *
         * Anchored to bytes_written (DMA-side ground truth) instead of
         * mcycle, so the output rate is stable regardless of how
         * mcycle pauses during WFI / MMIO stalls.  Set
         * KWS_PRINT_INTERVAL_MS=0 to fall back to the per-iter,
         * latest-debounce print path. */
        static uint32_t last_print_bw = 0;
        static int32_t  vote_score_sum[NUM_CLASSES] = {0};
        static uint32_t vote_n_clips = 0;

        /* Soft-vote accumulator: every iter contributes its int8
         * softmax row to the running total.  vote_n_clips lets the
         * print know how many inferences this winner represents.
         * Low-confidence clips (max_score < KWS_CONF_THRESH) are excluded
         * so they don't pollute the vote window. */
#ifndef KWS_DUMP_LAYERS
        if (max_score >= KWS_CONF_THRESH)
#endif
        {
            for (int v = 0; v < NUM_CLASSES; v++)
                vote_score_sum[v] += (int32_t)nnom_output_data[v];
            vote_n_clips++;
        }

#if KWS_PRINT_INTERVAL_MS > 0
        const int print_gate_open =
            (int32_t)(bytes_written - last_print_bw) >= (int32_t)KWS_PRINT_INTERVAL_RING_BYTES;
#else
        const int print_gate_open = 1;
#endif

        /* Resolve the winner from the accumulated soft scores when the
         * gate opens; otherwise nothing to print this iter.  We use
         * the soft-vote winner (not the per-clip debounced label) for
         * the emitted UART line so the output reflects the full
         * KWS_PRINT_INTERVAL_MS window, not just the last clip. */
        int vote_winner = NUM_CLASSES - 1;
        if (print_gate_open && (int)vote_n_clips >= KWS_MIN_VOTE_CLIPS) {
            int32_t best = vote_score_sum[0];
            vote_winner   = 0;
            for (int v = 1; v < NUM_CLASSES; v++) {
                if (vote_score_sum[v] > best) {
                    best = vote_score_sum[v];
                    vote_winner = v;
                }
            }
        }

#ifdef KWS_DUMP_LAYERS
        const int print_clip_base = 0;
#else
        /* Suppress "unknown" windows: skip when no clip cleared the
         * confidence gate (vote_n_clips==0) or the soft-vote winner is
         * the unknown class. */
        const int print_clip_base =
            ((int)vote_n_clips >= KWS_MIN_VOTE_CLIPS) && (vote_winner != (NUM_CLASSES - 1));
#endif
        const int print_clip = print_clip_base && print_gate_open;

        /* Snapshot the n_clips count before the reset (the print body
         * uses it to suffix the DETECT line so a downstream listener
         * can tell how confident the vote was). */
        const uint32_t voted_clips = vote_n_clips;

        /* Reset the accumulator + advance the print anchor on every
         * gate-open boundary, regardless of whether we actually
         * emitted (KWS_QUIET may suppress).  Otherwise consecutive
         * "unknown" windows would compound into a single huge sum
         * that biases the next genuine word's vote. */
        if (print_gate_open) {
            last_print_bw = bytes_written;
            vote_n_clips = 0;
            for (int v = 0; v < NUM_CLASSES; v++) vote_score_sum[v] = 0;
        }

        if (print_clip) {
            uart_puts("IRQS:");
            uart_putdec(i2s_irq_count);
            uart_puts("\r\n");
            uart_puts("DETECT:");
            uart_puts(class_names[vote_winner]);
            uart_puts(", ");
            uart_putdec(vote_winner);
            uart_puts(" (n=");
            uart_putdec((int)voted_clips);
            uart_puts(")\r\n");
        }
#ifdef USE_MCYCLE_CSR
        /* Three counters per clip:
         *   CYCLES_CAPTURE  I2S + DMA + ISR window (CPU mostly WFI).
         *   CYCLES_INFER    model_run() — the value the old "CYCLES:"
         *                    line reported.  Kept under that name too
         *                    for log-scraper backwards compatibility.
         *   CYCLES_TOTAL    capture + inference, the end-to-end cost
         *                    per clip (what matters for energy and
         *                    real-time latency budgets). */
        if (print_clip) {
            uart_puts("CYCLES_CAPTURE:");
            uart_putdec((int)cycles_capture);
            uart_puts("\r\nCYCLES_INFER:");
            uart_putdec((int)cycles);
            uart_puts("\r\nCYCLES_TOTAL:");
            uart_putdec((int)(cycles_capture + cycles));
            uart_puts("\r\nCYCLES:");      /* legacy alias = CYCLES_INFER */
            uart_putdec((int)cycles);
            uart_puts("\r\n");
        }

        /* Restart the capture-window timer for the next clip. */
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_capture_start));
#endif

#ifdef KWS_DUMP_LAYERS
        /* Single-shot dump: emit the spike-style PRED line and halt — no
         * second inference, no continued capture.  Final line matches
         * test/spikedebug/kws_spike_debug.c::printf("PRED:%d (%s)\n", ...). */
        uart_puts_lf("PRED:");
        uart_putu_lf((uint32_t)pred);
        uart_puts_lf(" (");
        uart_puts_lf(class_names[pred]);
        uart_puts_lf(")\n");
        while (1) asm volatile ("wfi");
#endif

        /* Dynamic sliding step — captured at the very end of the iter so
         * we count the *full* per-iteration wall time (snapshot loop +
         * AGC + inference + softmax + UART output), not just inference
         * cycles.  The DMA writes one ring byte every CYCLES_PER_RING_BYTE
         * cycles, so the next window's threshold has to advance by the
         * same many bytes the DMA wrote during this iter, otherwise
         * bytes_written drifts ahead and the audio-loss guard eventually
         * trips even though the ring isn't actually being overwritten.
         *
         * Bounded by:
         *   - KWS_MIN_STEP_RING_BYTES floor (default 20 ms) — gives idle
         *     time when iter is unusually short, so we don't burn CPU
         *     re-running inference faster than the user can read.
         *   - RING_SAMPLES_PER_CLIP ceiling — keeps consecutive windows
         *     contiguous (no audio gap); when iter wall time > 1 s
         *     anyway, the next WFI will detect the overflow and emit
         *     AUDIO_LOSS as before. */
        /* Ground-truth step from the DMA-side counter.  Always snap
         * bytes_written once (volatile) so an in-flight IRQ between
         * the read and the math can't widen the delta. */
        uint32_t bw_now     = bytes_written;
        uint32_t iter_bytes = bw_now - iter_start_bw;
#ifdef KWS_STEP_SAMPLES
        step_bytes = KWS_STEP_SAMPLES;
        (void)iter_bytes;   /* not used for step, but kept for the print below */
#elif defined(KWS_NO_OVERLAP) && KWS_NO_OVERLAP
        /* Non-overlapping mode: each window picks up exactly where the
         * previous one ended.  Pinning step_bytes to one full clip means
         * consecutive snapshots cover disjoint audio (no shared samples),
         * at the cost of higher worst-case detection latency.  Build with
         * -DKWS_NO_OVERLAP=1 to enable. */
        step_bytes = RING_SAMPLES_PER_CLIP;
        (void)iter_bytes;
#else
        step_bytes = iter_bytes;
        if (step_bytes < KWS_MIN_STEP_RING_BYTES) step_bytes = KWS_MIN_STEP_RING_BYTES;
        if (step_bytes > RING_SAMPLES_PER_CLIP)   step_bytes = RING_SAMPLES_PER_CLIP;
#endif
        if (print_clip) {
            uart_puts("ITER_BYTES:");
            uart_putdec((int)iter_bytes);
            uart_puts(" STEP:");
            uart_putdec((int)step_bytes);
            uart_puts(" BW:");
            uart_putdec((int)bw_now);
            uart_puts(" BAN:");
            uart_putdec((int)bytes_at_next_inference);
            uart_puts("\r\n");
        }
        bytes_at_next_inference += step_bytes;
    }

    return 0;
}
