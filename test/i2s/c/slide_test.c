/*
 * slide_test.c — sliding-window ring-buffer self-test.
 *
 * Goal: exercise the same sliding-window snapshot logic that
 * test/common/kws_bare_main.c uses, verify the ring buffer keeps a
 * valid window ready for every inference, and ensure each snapshot's
 * contents match the expected counter pattern across the slide.
 *
 * Approach:
 *   1. Drive the counter stimulus from sim/counter_32k.hex (32000 frames,
 *      q7 = n & 0xFF for sample n).
 *   2. Set up the same I2S + DMA + ring-buffer machinery as kws_bare_main.
 *   3. Loop SLIDE_ITERATIONS times.  In each iteration:
 *        a. Wait for bytes_written ≥ bytes_at_next_inference.
 *        b. Snapshot the most recent RING_SAMPLES_PER_CLIP ring bytes
 *           into snap[] (with the firmware ÷2 stride, like nnom_input_data).
 *        c. Verify the snapshot's first sample equals the expected counter
 *           position (== first_byte_index of that snapshot, modulo 256).
 *        d. Verify the snapshot's last sample equally.  If both ends
 *           match the counter pattern, the snapshot is byte-clean.
 *        e. Simulate inference with a busy-wait of SIM_INFERENCE_CYCLES.
 *        f. Compute step = max(MIN_STEP_RING_BYTES, sim_cycles / cycles_per_ring_byte)
 *           and advance bytes_at_next_inference by step.
 *
 *   The simulated inference duration is configurable: the default of
 *   1,080,000 cycles (~30 ms at 36 MHz) matches the measured
 *   mel_compact_int8_xip_accel inference, which yields a step of 480
 *   ring bytes — overlapping snapshots, the case the merge introduced.
 *
 * Output (single line per stage):
 *   SLIDE_PASS  iters=N  step=B
 * or
 *   SLIDE_FAIL  iter=K  pos=<first|last>  got=<x> exp=<y>  (snap_start=<n>  total=<n>  irqs=<n>)
 */

#include <stdint.h>
#include "uart.h"
#include "i2s_regs.h"
#include "MS_DMAC_AHBL.h"

/* ── I2S ─────────────────────────────────────────────────────────────────── */
#define I2S_BASE_ADDR  0x40008000UL
#define I2S_FIFO_PA    0x40008008UL
#define I2S            ((i2s_hw_t *)I2S_BASE_ADDR)

#ifndef I2S_DMA_BURST_WORDS
#  error "I2S_DMA_BURST_WORDS must be passed via -D"
#endif

#define DMA_BURST_BYTES        (I2S_DMA_BURST_WORDS * 4)
#define MODEL_HZ               8000
#define RING_HZ                (MODEL_HZ * 2)
#define MODEL_SAMPLES_PER_CLIP MODEL_HZ
#define RING_SAMPLES_PER_CLIP  RING_HZ

#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  17
#endif

#ifndef KWS_RING_SAMPLES
#define KWS_RING_SAMPLES 16384
#endif
#define KWS_RING_MASK (KWS_RING_SAMPLES - 1)

#define I2S_CONF_IRQ_EN     (1u << 4)
#define I2S_CONF_WIDTH_8    (2u << 6)

static void i2s_init(uint32_t clk_div) {
    I2S->conf = ((clk_div & 0xFFFFFFu) << 8) | I2S_CONF_IRQ_EN | I2S_CONF_WIDTH_8;
}

/* ── Sliding-window step config ─────────────────────────────────────────── */
/* Same parameters kws_bare_main now uses. */
#ifndef KWS_MIN_STEP_RING_BYTES
#define KWS_MIN_STEP_RING_BYTES (RING_HZ / 50)   /* 20 ms at 16 kHz = 320 */
#endif
#ifdef CLK_MHZ
#define CYCLES_PER_RING_BYTE ((uint32_t)((CLK_MHZ) * 1000000UL) / RING_HZ)
#else
#define CYCLES_PER_RING_BYTE 2250u
#endif

/* Simulated inference cycle budget per slide.  Default ≈ accel mel_compact_
 * int8 (1.08 M cycles ≈ 30 ms at 36 MHz) so the test exercises the
 * overlapping-snapshot case.  Override per build with -DSIM_INFERENCE_CYCLES. */
#ifndef SIM_INFERENCE_CYCLES
#define SIM_INFERENCE_CYCLES 1080000u
#endif

#ifndef SLIDE_ITERATIONS
#define SLIDE_ITERATIONS 10
#endif

/* Phantom-sample prefix: receiver fires its first STARTUP_PHANTOMS WS↓
 * pulses before mic_sim has loaded a real frame, so audio_ring[0..N-1]
 * holds the initial m_current_word=0 bytes.  Same constant used by
 * dataflow_test.c. */
#ifndef STARTUP_PHANTOMS
#define STARTUP_PHANTOMS 2
#endif

/* ── DMAC ────────────────────────────────────────────────────────────────── */
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

#define DMAC_CTRL_I2S_PIRQ \
    ((1u <<  0) | (1u <<  8) | (2u << 16) | (0u << 18) | (2u << 24) | (4u << 26))

__attribute__((aligned(4))) static volatile int8_t audio_ring[KWS_RING_SAMPLES];
static int8_t snap[MODEL_SAMPLES_PER_CLIP];

static void dma_arm(void) {
    DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u;
    DMAC->saddr   = I2S_FIFO_PA;
    DMAC->daddr   = (uint32_t)(uintptr_t)audio_ring;
    DMAC->count   = (uint32_t)(I2S_DMA_BURST_WORDS - 1);
    DMAC->icr     = 1u;
    DMAC->control = DMAC_CTRL_I2S_PIRQ;
}

/* ── CSR helpers ─────────────────────────────────────────────────────────── */
static inline void csr_set_mtvec(void (*handler)(void)) {
    asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)handler));
}
static inline void csr_enable_meie(void) { asm volatile ("csrs mie, %0" :: "r"(1u << 11)); }
static inline void csr_enable_mie(void)  { asm volatile ("csrsi mstatus, 8"); }
static inline void csr_meiea_dmac_en(void) {
    asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16));
}
static inline void csr_enable_cycle_counter(void) {
    /* Hazard3 inhibits mcycle by default; clear mcountinhibit so it runs. */
    asm volatile ("csrw 0x320, zero");
}

/* ── Capture state ───────────────────────────────────────────────────────── */
static volatile uint32_t bytes_written = 0;
static volatile uint32_t dmac_irq_count = 0;

void __attribute__((interrupt("machine"), aligned(4))) slide_trap_handler(void) {
    uint32_t mcause;
    asm volatile ("csrr %0, mcause" : "=r"(mcause));
    if (!(mcause & 0x80000000u)) {
        uart_printf("EXCEPTION cause=%x\r\n", (uint32_t)(mcause & 0x7fffffffu));
        while (1);
    }
    DMAC->icr = 1u;
    uint32_t next_daddr = DMAC->daddr + DMA_BURST_BYTES;
    if (next_daddr >= (uint32_t)(uintptr_t)audio_ring + KWS_RING_SAMPLES)
        next_daddr = (uint32_t)(uintptr_t)audio_ring;
    DMAC->daddr = next_daddr;
    bytes_written += DMA_BURST_BYTES;
    dmac_irq_count++;
    (void)DMAC->control;
}

/* Busy-loop simulating model_run() of fixed cycle cost. */
static void sim_inference(uint32_t cycles) {
    uint32_t start, now;
    asm volatile ("csrr %0, mcycle" : "=r"(start));
    do {
        asm volatile ("csrr %0, mcycle" : "=r"(now));
    } while ((uint32_t)(now - start) < cycles);
}

/* ── main ────────────────────────────────────────────────────────────────── */
int main(void) {
    uart_init();
    uart_printf("SLIDE_TEST start (cfg_div=%x burst=%x ring=%x sim_cyc=%x min_step=%x iters=%x)\r\n",
                (uint32_t)I2S_CLK_DIV,
                (uint32_t)I2S_DMA_BURST_WORDS,
                (uint32_t)KWS_RING_SAMPLES,
                (uint32_t)SIM_INFERENCE_CYCLES,
                (uint32_t)KWS_MIN_STEP_RING_BYTES,
                (uint32_t)SLIDE_ITERATIONS);

    csr_enable_cycle_counter();
    csr_set_mtvec(slide_trap_handler);
    csr_enable_meie();
    csr_meiea_dmac_en();
    dma_arm();
    i2s_init(I2S_CLK_DIV);
    csr_enable_mie();

    uint32_t bytes_at_next_inference = RING_SAMPLES_PER_CLIP;
    uint32_t step_bytes              = RING_SAMPLES_PER_CLIP;
    uint32_t audio_loss_count        = 0;

    for (uint32_t iter = 0; iter < SLIDE_ITERATIONS; iter++) {
        /* Wait for the next snapshot's worth of audio. */
        while ((int32_t)(bytes_written - bytes_at_next_inference) < 0) {
            asm volatile ("wfi");
        }

        /* Audio-loss guard: if the firmware has fallen more than one
         * clip behind, the ring has overwritten bytes we still need.
         * Print a notice and resync (mirrors the production logic). */
        if ((int32_t)(bytes_written - bytes_at_next_inference) > (int32_t)RING_SAMPLES_PER_CLIP) {
            uint32_t lost = (bytes_written - bytes_at_next_inference) - RING_SAMPLES_PER_CLIP;
            uart_printf("AUDIO_LOSS bytes=%x\r\n", lost);
            audio_loss_count++;
            bytes_at_next_inference = bytes_written;
        }

        /* Snapshot.  Anchor to bytes_at_next_inference, NOT bytes_written,
         * so the window position tracks the explicit step. */
        uint32_t snap_start_byte = bytes_at_next_inference - RING_SAMPLES_PER_CLIP;
        uint32_t snap_start_ring = snap_start_byte & KWS_RING_MASK;
        for (int j = 0; j < MODEL_SAMPLES_PER_CLIP; j++) {
            uint32_t ring_idx = (snap_start_ring + (uint32_t)(j * 2)) & KWS_RING_MASK;
            snap[j] = audio_ring[ring_idx];
        }

        /* Expected counter values at the start and end of this snapshot.
         * snap_start_byte is the absolute byte index (cumulative) where
         * the snapshot begins.  Subtracting STARTUP_PHANTOMS gives the
         * stim-sample index for that ring byte; that index modulo 256
         * is the q7 we expect to see. */
        uint32_t first_stim_idx = (snap_start_byte >= STARTUP_PHANTOMS)
                                    ? (snap_start_byte - STARTUP_PHANTOMS)
                                    : 0;
        uint8_t  exp_first_byte = (uint8_t)(first_stim_idx & 0xFF);
        if (iter == 0 && snap_start_byte < STARTUP_PHANTOMS) {
            exp_first_byte = 0;     /* Snapshot starts inside phantom prefix. */
        }

        /* Last byte of the snapshot: snap[MODEL_SAMPLES_PER_CLIP - 1]
         * reads ring[snap_start_ring + (MODEL_SAMPLES_PER_CLIP-1)*2]. */
        uint32_t last_ring_byte_idx = snap_start_byte + (MODEL_SAMPLES_PER_CLIP - 1) * 2;
        uint8_t  exp_last_byte      = (uint8_t)((last_ring_byte_idx - STARTUP_PHANTOMS) & 0xFF);

        uint8_t got_first = (uint8_t)snap[0];
        uint8_t got_last  = (uint8_t)snap[MODEL_SAMPLES_PER_CLIP - 1];

        if (got_first != exp_first_byte) {
            uart_printf("SLIDE_FAIL  iter=%x pos=first  got=%x exp=%x  (snap_start=%x  bytes_at_next=%x)\r\n",
                        iter, (uint32_t)got_first, (uint32_t)exp_first_byte,
                        snap_start_ring, bytes_at_next_inference);
            asm volatile ("ebreak"); while (1);
        }
        if (got_last != exp_last_byte) {
            uart_printf("SLIDE_FAIL  iter=%x pos=last   got=%x exp=%x  (snap_start=%x  bytes_at_next=%x)\r\n",
                        iter, (uint32_t)got_last, (uint32_t)exp_last_byte,
                        snap_start_ring, bytes_at_next_inference);
            asm volatile ("ebreak"); while (1);
        }

        uart_printf("ITER %x: bytes_at_next=%x snap_start=%x first=%x last=%x step=%x irqs=%x\r\n",
                    iter, bytes_at_next_inference, snap_start_ring,
                    (uint32_t)got_first, (uint32_t)got_last,
                    step_bytes, (uint32_t)dmac_irq_count);

        /* Simulate inference. */
        sim_inference(SIM_INFERENCE_CYCLES);

        /* Compute dynamic step (same math as kws_bare_main). */
        step_bytes = (SIM_INFERENCE_CYCLES + CYCLES_PER_RING_BYTE - 1) / CYCLES_PER_RING_BYTE;
        if (step_bytes < KWS_MIN_STEP_RING_BYTES) step_bytes = KWS_MIN_STEP_RING_BYTES;
        if (step_bytes > RING_SAMPLES_PER_CLIP)   step_bytes = RING_SAMPLES_PER_CLIP;

        bytes_at_next_inference += step_bytes;
    }

    uint32_t step_report = step_bytes;

    uart_printf("SLIDE_PASS  iters=%x step=%x audio_losses=%x  bytes_written=%x  irqs=%x\r\n",
                (uint32_t)SLIDE_ITERATIONS,
                step_report,
                audio_loss_count,
                bytes_written,
                (uint32_t)dmac_irq_count);
    asm volatile ("ebreak");
    while (1);
    return 0;
}
