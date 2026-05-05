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
 *  i2s_irq (FIFO half-full) → CPU trap (IRQ 2) → ISR writes swtrig=1
 *  → DMA burst reads I2S_DMA_BURST_WORDS words into audio_ring
 *  → dmac_irq (IRQ 0) → ISR advances daddr, bumps bytes_written.
 *  Main waits via WFI until the ring holds a full second, snapshots
 *  it into nnom_input_data, runs inference, prints DETECT/CYCLES/IRQS.
 *
 *  HW Q8 in apb_i2s_receiver packs LSB=oldest, so on this little-
 *  endian RV32 each 32-bit AHB write lands in time order — no
 *  byte-shuffle in the ISR, no end-of-clip ring_buf memcpy.  Saves
 *  ~58K cycles per clip and 8 KB of staging SRAM.  The weights
 *  header annotates nnom_input_data with aligned(4); the
 *  scripts/align_nnom_input.sh helper re-applies it on regen.
 *
 * REGISTER MAP (KWS-SoC)
 *  UART  0x40004000   uart_mini (115200 8N1, CLK_MHZ FPGA / sim)
 *  I2S   0x40008000   apb_i2s_receiver (INMP441 MEMS mic, 16 kHz mono)
 *  DMAC  0x60000000   MS_DMAC_AHBL (SW-triggered by i2s_irq CPU ISR)
 *
 * CLASSES (index order must match the weights header):
 *  0=down 1=go 2=left 3=no 4=off 5=on 6=right 7=stop 8=up 9=yes 10=unknown
 */

#include <stdint.h>
#include <string.h>

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

/* ── I2S (KWS-SoC apb_i2s_receiver at 0x40008000) ──────────────────────── */

typedef struct {
    volatile uint32_t id;    /* 0x00: peripheral ID (ROV = 0xDEADCAFE)        */
    volatile uint32_t conf;  /* 0x04: [31:8] cfg_div
                              *       [7]    cfg_skip_r_en   (skip-R-SCK,
                              *                               see RTL patch)
                              *       [6]    cfg_q8_en       (HW int8 quant
                              *                               4 samples/word,
                              *                               MSB = oldest)
                              *       [5]    cfg_ds_en       (HW 2x downsample)
                              *       [4]    cfg_irq_en                       */
    volatile uint32_t fifo;  /* 0x08: audio FIFO; with q8_en=1 each 32-bit
                              *       read returns 4 packed int8 samples
                              *       (byte[3]=oldest, byte[0]=newest).       */
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
 * system cycles, so the SCK PERIOD is 2 * (cfg_div + 1) cycles.  One
 * stereo frame is 64 SCK ticks (32-bit L + 32-bit R), and we sample
 * the L channel only (MONO_MODE), so:
 *
 *   sample_rate = clk / (128 * (cfg_div + 1))
 *
 *   36 MHz FPGA, ~48 kHz raw → cfg_div = 6
 *   12 MHz sim,  ~47 kHz raw → cfg_div = 1
 *
 * DS_EN with 3× decimation (apb_i2s_receiver.v) keeps 1 of every 3 samples
 * → effective 16 kHz.
 *
 * Override on the make line (e.g. -DI2S_CLK_DIV=1 for 12 MHz sim). */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  6
#endif

/* I2S_CONF bit masks — match peris/i2s/i2s_apb/i2s_itr2/i2s_regs.h */
#define I2S_CONF_IRQ_EN     (1u << 4)
#define I2S_CONF_DS_EN      (1u << 5)   /* HW 3x downsample (keep 1 of 3)    */
#define I2S_CONF_Q8_EN      (1u << 6)   /* HW int8 quant: 4 samples per word */
#define I2S_CONF_SKIP_R_EN  (1u << 7)   /* skip SCK toggling during R slot   */

static void i2s_init(uint32_t clk_div) {
    /* Q8_EN: HW packs 4 int8 samples per FIFO word — 4× less FIFO/AHB
     *        traffic per second of audio.  The trap handler unpacks
     *        MSB-first (byte[3] of dma_batch[i] is the OLDEST sample).
     * SKIP_R_EN: I2S protocol forces SCK toggling through both L and R
     *        slots even in MONO_MODE (we throw R away).  This bit gates
     *        SCK during the R slot, halving SCK toggle activity (mic +
     *        I2S peripheral) for free.  Requires the matching RTL patch
     *        in peris/i2s/i2s_apb/i2s_itr2/apb_i2s_receiver.v.
     * DS_EN: enabled — with cfg_div=6 the raw rate is ~48 kHz; DS_EN
     *        keeps 1 of every 3 samples (3× decimation) → ~16 kHz effective. */
    I2S->conf = ((clk_div & 0xFFFFFFu) << 8)
              | I2S_CONF_IRQ_EN
              | I2S_CONF_DS_EN
              | I2S_CONF_Q8_EN
              | I2S_CONF_SKIP_R_EN;
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

/* SW-triggered word transfer: fixed source (FIFO), incrementing dest.
 *   bit 0      EN=1
 *   bits 11:8  TRIGGER = 0 (SW — i2s_irq CPU ISR writes swtrig=1)
 *   bits 17:16 STYPE   = 2 (word)
 *   bits 20:18 SAI     = 0 (no source increment — FIFO)
 *   bits 25:24 DTYPE   = 2 (word)
 *   bits 28:26 DAI     = 4 (+4B per word) */
#define DMAC_CTRL_I2S_SW \
    ((1u <<  0) | (0u <<  8) | (2u << 16) | (0u << 18) | (2u << 24) | (4u << 26))

#define DMA_BURST_BYTES   (I2S_DMA_BURST_WORDS * 4)
#define SAMPLES_PER_CLIP  16000    /* 1 second at 16 kHz, int8 Q7 */
#define NUM_CLASSES       11

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

/* Sliding step: bytes between consecutive inferences.
 *   - Today (4 s inference): SAMPLES_PER_CLIP = no overlap; inferences
 *     run back-to-back as fast as model_run allows.
 *   - With accelerator (~100 ms inference): set to 320-640 (= 20-40 ms
 *     at 16 kHz) to detect words straddling natural clip boundaries. */
#ifndef KWS_STEP_SAMPLES
#define KWS_STEP_SAMPLES SAMPLES_PER_CLIP
#endif

__attribute__((aligned(4))) static volatile int8_t audio_ring[KWS_RING_SAMPLES];

static void dma_arm(void) {
    DMAC->control = DMAC_CTRL_I2S_SW & ~1u;          /* EN=0 while reconfiguring */
    DMAC->saddr   = I2S_FIFO_PA;
    DMAC->daddr   = (uint32_t)(uintptr_t)audio_ring;
    DMAC->count   = (uint32_t)(I2S_DMA_BURST_WORDS - 1); /* COUNT = N-1 for N transfers */
    DMAC->control = DMAC_CTRL_I2S_SW;                /* EN=1 — armed for SW trigger */
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
static inline uint32_t csr_meipa_read(void) {
    /* Hazard3 XH3IRQ MEIPA (0xfe0): pending external IRQ bits for group 0.
     * bit 16 = dmac_irq (IRQ 0), bit 17 = uart_irq (IRQ 1), bit 18 = i2s_irq (IRQ 2). */
    uint32_t v;
    asm volatile ("csrr %0, 0xfe0" : "=r"(v));
    return v;
}
static inline void csr_meiea_kws_en(void) {
    /* Unmask dmac_irq (IRQ 0, bit 16) and i2s_irq (IRQ 2, bit 18) in
     * Hazard3 MEIEA group 0.  A single csrw replaces the whole group mask,
     * so both bits must be set together.
     * kws_soc.v: .irq({i2s_irq, uart_irq, dmac_irq}) — IRQ 2 = i2s_irq. */
    asm volatile ("csrw 0xbe0, %0" :: "r"((1u << 16) | (1u << 18)));
}
/* Mask i2s_irq while a DMA burst is in flight: keeps dmac_irq enabled so the
 * burst-complete handler can re-arm.  i2s_irq is level-sensitive — without this
 * mask the CPU would spin in the ISR and starve the DMA of AHB bus access. */
static inline void csr_meiea_kws_i2s_dis(void) {
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
 *  Dispatches on Hazard3 XH3IRQ MEIPA (CSR 0xfe0, group 0):
 *
 *  i2s_irq path (IRQ 2, MEIPA bit 18):
 *  1. i2s_irq asserts (FIFO half-full) → CPU trap → ISR writes swtrig=1,
 *     then masks i2s_irq in MEIEA (keeps only dmac_irq enabled).
 *     Masking is required because i2s_irq is level-sensitive: without it
 *     the CPU would spin re-entering the ISR and starve the DMA of AHB access.
 *
 *  dmac_irq path (IRQ 0, MEIPA bit 16):
 *  3. DMA burst completes → dmac_irq → ISR clears ICR, advances daddr
 *     by one burst (wrapping at the end of audio_ring), bumps bytes_written.
 *  4. The read-back of DMAC->control is an AHB fence ensuring the ICR
 *     write reaches the peripheral before mret so Hazard3 does not
 *     immediately re-trap on the still-posted IRQ edge.
 *
 *  dmac is handled before i2s: if both bits are pending simultaneously
 *  (burst completed while entering the handler), DADDR is updated before
 *  the next swtrig fires so the new burst lands in the correct ring slot.
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

    uint32_t meipa = csr_meipa_read();

    if (meipa & (1u << 16)) {
        /* dmac_irq: burst complete — advance ring write pointer, re-arm i2s_irq */
        DMAC->icr = 1u;

        uint32_t next_daddr = DMAC->daddr + DMA_BURST_BYTES;
        if (next_daddr >= (uint32_t)(uintptr_t)audio_ring + KWS_RING_SAMPLES)
            next_daddr = (uint32_t)(uintptr_t)audio_ring;
        DMAC->daddr = next_daddr;

        bytes_written += DMA_BURST_BYTES;
        i2s_irq_count++;

        (void)DMAC->control;   /* AHB read-back fence */
        csr_meiea_kws_en();    /* re-enable i2s_irq for next burst */
    }

    if (meipa & (1u << 18)) {
        /* i2s_irq: FIFO half-full — fire one burst, mask i2s_irq until burst done */
        DMAC->swtrig = 1u;
        csr_meiea_kws_i2s_dis();
    }
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
     * starts at SAMPLES_PER_CLIP — the first inference fires once the
     * ring holds a full second of audio — and advances by KWS_STEP_SAMPLES
     * after each inference (= SAMPLES_PER_CLIP today, smaller when a
     * Conv1D accelerator makes overlapping windows feasible). */
    uint32_t bytes_at_next_inference = SAMPLES_PER_CLIP;
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

        /* Snapshot the most recent SAMPLES_PER_CLIP bytes from the ring
         * into nnom_input_data, handling the wrap.  We freeze a `total`
         * value first so that even if the DMA bumps bytes_written during
         * the memcpy we use a consistent window position; the ring is
         * sized so the DMA can't overtake the snapshot region during
         * the copy (see KWS_RING_SAMPLES rationale above). */
        uint32_t total = bytes_written;
        uint32_t snap_start_ring = (total - SAMPLES_PER_CLIP) & KWS_RING_MASK;
        if (snap_start_ring + SAMPLES_PER_CLIP <= KWS_RING_SAMPLES) {
            memcpy(nnom_input_data,
                   (const void *)&audio_ring[snap_start_ring],
                   (size_t)SAMPLES_PER_CLIP);
        } else {
            uint32_t first = KWS_RING_SAMPLES - snap_start_ring;
            memcpy(nnom_input_data,
                   (const void *)&audio_ring[snap_start_ring],
                   (size_t)first);
            memcpy(&nnom_input_data[first],
                   (const void *)&audio_ring[0],
                   (size_t)(SAMPLES_PER_CLIP - first));
        }

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
            for (int k = 0; k < SAMPLES_PER_CLIP; k++) {
                uint32_t word = ((uint32_t)(int32_t)nnom_input_data[k] << 16)
                                & 0xffff0000u;
                uart_puthex(word);
                uart_puts("\r\n");
            }
            uart_puts("RB_END\r\n");
            asm volatile ("ebreak");
        }
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

        if (pred == last_pred) {
            debounce_cnt++;
        } else {
            last_pred    = pred;
            debounce_cnt = 1;
        }
        int debounced = (debounce_cnt >= KWS_DEBOUNCE_COUNT) ? pred : (NUM_CLASSES - 1);

        uart_puts("IRQS:");
        uart_putdec(i2s_irq_count);
        uart_puts("\r\n");
        uart_puts("DETECT:");
        uart_puts(class_names[debounced]);
        uart_puts(", ");
        uart_putdec(debounced);
        uart_puts("\r\n");
#ifdef USE_MCYCLE_CSR
        /* Three counters per clip:
         *   CYCLES_CAPTURE  I2S + DMA + ISR window (CPU mostly WFI).
         *   CYCLES_INFER    model_run() — the value the old "CYCLES:"
         *                    line reported.  Kept under that name too
         *                    for log-scraper backwards compatibility.
         *   CYCLES_TOTAL    capture + inference, the end-to-end cost
         *                    per clip (what matters for energy and
         *                    real-time latency budgets). */
        uart_puts("CYCLES_CAPTURE:");
        uart_putdec((int)cycles_capture);
        uart_puts("\r\nCYCLES_INFER:");
        uart_putdec((int)cycles);
        uart_puts("\r\nCYCLES_TOTAL:");
        uart_putdec((int)(cycles_capture + cycles));
        uart_puts("\r\nCYCLES:");      /* legacy alias = CYCLES_INFER */
        uart_putdec((int)cycles);
        uart_puts("\r\n");

        /* Restart the capture-window timer for the next clip. */
        asm volatile ("csrr %0, mcycle" : "=r"(cyc_capture_start));
#endif
    }

    return 0;
}
