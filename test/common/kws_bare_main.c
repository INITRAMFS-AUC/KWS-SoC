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
 *   36 MHz FPGA, ~40-48 kHz raw → cfg_div = 6   (40.2 kHz)
 *   12 MHz sim,  ~47 kHz raw    → cfg_div = 1
 *
 * HW cfg_ds_en (3× decimation) drops to ~16 kHz; the snapshot loop in main
 * does an additional ÷2 to land at the model's 8 kHz int8 input.
 *
 * Override on the make line (e.g. -DI2S_CLK_DIV=1 for 12 MHz sim). */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  6
#endif

/* I2S_CONF bit masks — match peris/i2s/i2s_apb/i2s_itr2/i2s_regs.h.
 * WIDTH[7:6]: 0=32-bit (1 sample/word), 1=16-bit (2/word), 2=8-bit (4/word). */
#define I2S_CONF_IRQ_EN     (1u << 4)
#define I2S_CONF_DS_EN      (1u << 5)   /* HW 3x downsample (keep 1 of 3)    */
#define I2S_CONF_WIDTH_8    (2u << 6)   /* 4 int8 samples per FIFO word      */

/* KWS_DS_EN: 1 = enable HW ÷3 decimation (default — assumes ~48 kHz raw
 *            mic, dropped to ~16 kHz at the FIFO).
 *            0 = disable HW decimation; the ring rate equals the raw rate.
 *                Use when the mic is already at the model's expected rate
 *                (e.g. mel_compact_int8_peak_norm: 16 kHz mic, 16 kHz model
 *                — no decimation needed in either HW or firmware). */
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
     * DS_EN:   enabled (KWS_DS_EN=1) — HW keeps 1 of every 3 raw samples
     *          (3× decimation), turning a ~48 kHz mic into ~16 kHz int8 in
     *          audio_ring.  Disabled (KWS_DS_EN=0) when the mic is already
     *          at the target ring rate (peak-norm 16 kHz model). */
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
/* RING_HZ: rate the receiver delivers into audio_ring after HW ÷3 decimation
 * (48 kHz mic → 16 kHz ring).  Fixed by the I2S pipeline.
 *
 * KWS_MODEL_HZ: rate the model expects.  For the original 8 kHz models,
 * the snapshot decimates ÷2 from the ring; for 16 kHz models (e.g. the
 * peak-norm variant), no decimation — every ring sample feeds the model.
 *
 * MODEL_SAMPLES_PER_CLIP: must match nnom_input_data[] in the weights
 * header (currently 8000 across all variants → 1 s @ 8 kHz, 0.5 s @ 16 kHz).
 *
 * Override via -DKWS_MODEL_HZ=16000 on the model's CFLAGS line. */
#define RING_HZ                16000
#ifndef KWS_MODEL_HZ
#define KWS_MODEL_HZ           8000
#endif
#define KWS_DECIMATE           (RING_HZ / KWS_MODEL_HZ)   /* 1 or 2 */
#define MODEL_SAMPLES_PER_CLIP 8000
#define RING_SAMPLES_PER_CLIP  (MODEL_SAMPLES_PER_CLIP * KWS_DECIMATE)
#define NUM_CLASSES            11

/* ── Per-clip peak normalization ─────────────────────────────────────────────
 * Applied to nnom_input_data[] after the snapshot loop.
 * Matches the Python training pipeline exactly:
 *   scale = min(96 / peak, 64)   where peak = max(|s|) over the clip
 *   out[i] = clip(round(s * scale), -128, 127)
 * Using integer arithmetic: for peak>=2, scaled = (s*96 + half) / peak.
 * When peak==1 the cap applies: scaled = s*64.
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

/* Sliding step: ring-rate bytes between consecutive inferences.  At the ring
 * rate (16 kHz int8), RING_SAMPLES_PER_CLIP = no overlap; smaller values let
 * a fast accelerator catch words straddling clip boundaries. */
#ifndef KWS_STEP_SAMPLES
#define KWS_STEP_SAMPLES RING_SAMPLES_PER_CLIP
#endif

__attribute__((aligned(4))) static volatile int8_t audio_ring[KWS_RING_SAMPLES];

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
#ifdef USE_MCYCLE_CSR
static inline void csr_enable_cycle_counter(void) {
    /* Hazard3 inhibits mcycle by default; clear mcountinhibit so it runs. */
    asm volatile ("csrw 0x320, zero");
}
#endif
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

#ifdef USE_MCYCLE_CSR
    csr_enable_cycle_counter();
#endif
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

    /* `bytes_at_next_inference` is the bytes_written threshold the main
     * loop waits on before snapshotting the next 1-second window.  It
     * starts at RING_SAMPLES_PER_CLIP (= 16000 ring-rate bytes = 1 s of
     * audio) and advances by KWS_STEP_SAMPLES after each inference. */
    uint32_t bytes_at_next_inference = RING_SAMPLES_PER_CLIP;
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
        /* Wait until we have enough new bytes for the next inference. */
        while ((int32_t)(bytes_written - bytes_at_next_inference) < 0) {
            asm volatile ("wfi");
        }

#ifdef USE_MCYCLE_CSR
        uint32_t cyc_capture_end;
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_capture_end));
        uint32_t cycles_capture = cyc_capture_end - cyc_capture_start;
#endif

        /* Snapshot the most recent RING_SAMPLES_PER_CLIP bytes from the
         * ring AND decimate ÷2 into nnom_input_data — receiver delivers
         * 16 kHz int8, model wants 8 kHz int8, so we keep every other
         * sample.  Freeze `total` first so that DMA progress during the
         * snapshot doesn't shift the window; the ring is sized so the
         * DMA can't overtake the snapshot region during the copy
         * (see KWS_RING_SAMPLES rationale above). */
        uint32_t total = bytes_written;
        uint32_t snap_start_ring = (total - RING_SAMPLES_PER_CLIP) & KWS_RING_MASK;
        for (int j = 0; j < MODEL_SAMPLES_PER_CLIP; j++) {
            uint32_t ring_idx =
                (snap_start_ring + (uint32_t)(j * KWS_DECIMATE)) & KWS_RING_MASK;
            nnom_input_data[j] = (int8_t)audio_ring[ring_idx];
        }
        peak_norm_clip(nnom_input_data, MODEL_SAMPLES_PER_CLIP);

        bytes_at_next_inference += KWS_STEP_SAMPLES;

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
        /* Quiesce the audio path before inference.  In streaming mode the
         * I2S/DMA pipe keeps pumping audio_ring while model_run is busy;
         * the Conv1D accelerator writes activations into the same SRAM and
         * reads weights via XIP, so DMA/accel contention on the AHB bus
         * stalls inference badly (observed: stuck mid-LAYER_8 for minutes
         * in sim).  We only need one inference for the dump — disable mic,
         * EN=0 the DMA, mask the dmac_irq, and let the accelerator run
         * unobstructed. */
        I2S->conf = 0;             /* stop SCK / WS, halts the FIFO push */
        DMAC->control &= ~1u;      /* DMA EN=0 — drop any pending PIRQ */
        asm volatile ("csrci mstatus, 8");  /* global IRQs off */
#endif
#ifdef USE_MCYCLE_CSR
        uint32_t cyc_start, cyc_end;
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_start));
#endif
        model_run(model);
#ifdef USE_MCYCLE_CSR
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_end));
        uint32_t cycles = cyc_end - cyc_start;
#endif

        int    pred      = 0;
        int8_t max_score = nnom_output_data[0];
        for (int j = 1; j < NUM_CLASSES; j++) {
            if (nnom_output_data[j] > max_score) {
                max_score = nnom_output_data[j];
                pred      = j;
            }
        }

#ifndef KWS_DUMP_LAYERS
        /* Confidence-threshold gate: anything below ~30 % softmax peak is
         * downgraded to "unknown" to suppress nuisance detections in the
         * production stream.  The spike-debug reference binary doesn't apply
         * this gate, so we skip it in dump mode to keep the PRED line
         * comparable byte-for-byte. */
        if (max_score < 38) {
          pred = 10;
        }
#endif

        if (pred == last_pred) {
            debounce_cnt++;
        } else {
            last_pred    = pred;
            debounce_cnt = 1;
        }
        int debounced = (debounce_cnt >= KWS_DEBOUNCE_COUNT) ? pred : (NUM_CLASSES - 1);

        /* KWS_QUIET: only print clips that resolved to a *real* word — i.e.
         * the post-debounce label is not "unknown" (the last class).  Useful
         * when running long captures of mostly-silence over a slow UART:
         * no per-clip line storm, only meaningful detections.  The full
         * per-clip stream is still available by leaving the flag unset. */
#ifdef KWS_DUMP_LAYERS
        /* Single-shot dump build — suppress the IRQS/DETECT/CYCLES preamble
         * so the UART stream is *only* the layer-dump callback's output
         * plus the final PRED line below.  Diff-friendly. */
        const int print_clip = 0;
#elif defined(KWS_QUIET)
        int print_clip = (debounced != (NUM_CLASSES - 1));
#else
        const int print_clip = 1;
#endif

        if (print_clip) {
            uart_puts("IRQS:");
            uart_putdec(i2s_irq_count);
            uart_puts("\r\n");
            uart_puts("DETECT:");
            uart_puts(class_names[debounced]);
            uart_puts(", ");
            uart_putdec(debounced);
            uart_puts("\r\n");
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
    }

    return 0;
}
