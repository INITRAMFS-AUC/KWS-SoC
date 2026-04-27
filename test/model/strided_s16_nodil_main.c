/**
 * kws_bare.c — Bare-metal KWS inference firmware for KWS-SoC
 *
 * This firmware runs NNoM int8 keyword-spotting inference on the KWS-SoC
 * (Hazard3 RV32IMAC, 128 KB SRAM, I2S mic, UART). It also runs on Spike
 * with the spike_uart and spike_i2s MMIO plugins (see plugins/).
 *
 * The SAME compiled binary targets both environments. On Spike, the MMIO
 * plugins intercept register accesses at the SoC addresses. On the real
 * SoC, the actual hardware responds.
 *
 * PIPELINE
 * --------
 *  1. UART init — prints status messages
 *  2. NNoM model init — builds graph, uses 64 KB static buffer
 *  3. I2S config — start receiver, enable interrupt
 *  4. DMA config — PIRQ-triggered: I2S FIFO → dma_batch staging buffer
 *  5. Interrupt setup — mtvec, MEIE, MEIEA (XH3IRQ=1), MIE
 *  6. Inference loop (WFI):
 *       - i2s_irq asserts (FIFO full) → DMA hardware burst, no CPU involvement
 *       - dmac_irq fires AFTER burst completes (FC_REG=1 mode)
 *       - ISR: dma_batch already populated, extract int8 Q7, re-arm DMA
 *       - When ring full: copy to NNoM input, re-arm DMA, run inference
 *         (audio capture continues during inference via DMA+ISR)
 *
 * REGISTER MAP (KWS-SoC)
 * ----------------------
 *  UART  0x40004000   uart_mini (115200 8N1 at 36 MHz FPGA / 12 MHz sim)
 *  I2S   0x40008000   apb_i2s_receiver (INMP441 MEMS mic, 8 kHz mono)
 *  DMAC  0x60000000   MS_DMAC_AHBL (PIRQ[0]=i2s_irq)
 *
 * BUILD
 * -----
 *  make build/kws_bare        — cross-compile for bare-metal Spike/SoC
 *  make run_kws_bare AUDIO_FILE=test_clips/yes_001.bin
 *                             — run on Spike with audio input
 *
 * AUDIO INPUT FORMAT
 * ------------------
 *  Raw binary: one int8 Q7 sample per byte, 8000 bytes per second.
 *  Generate with: python3 scripts/gen_spike_audio.py
 *
 * UART OUTPUT (per inference)
 * --------------------------
 *  On Spike (single clip): "DETECT:<class_index>,<class_name>\r\n"
 *  On SoC (continuous):    "DETECT:<class_index>,<class_name>\r\n"
 *
 * CLASSES (index order matches strided_s16_nodil_weights.h)
 * ----------------------------------------
 *  0=down 1=go 2=left 3=no 4=off 5=on 6=right 7=stop 8=up 9=yes 10=unknown
 */

#include <stdint.h>
#include <string.h>
#include "strided_s16_nodil_weights.h"   /* NNoM model: nnom_model_create(), nnom_input_data[],
                                          * nnom_output_data[], pulled in with nnom.h */

/* ── UART (KWS-SoC uart_mini at 0x40004000) ─────────────────────────────── */

#define UART_BASE            0x40004000UL
#define UART_CSR_OFFS        0x00
#define UART_DIV_OFFS        0x04
#define UART_FSTAT_OFFS      0x08
#define UART_TX_OFFS         0x0C
#define UART_REG(off)        (*(volatile uint32_t *)(UART_BASE + (off)))
#define UART_CSR_EN_MASK     (1u << 0)
#define UART_FSTAT_TXFULL    (1u << 8)

/* CLK_MHZ and UART_BAUD_RATE injected by Makefile (-D flags) */
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
    /* Spike plugin always returns FSTAT=0 (not full), so this never spins */
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
    volatile uint32_t id;    /* 0x00: peripheral ID (ROV = 0xDEADCAFE) */
    volatile uint32_t conf;  /* 0x04: [31:8]=clk_div, [4]=irq_en */
    volatile uint32_t fifo;  /* 0x08: audio FIFO (read one 32-bit sample) */
} i2s_hw_t;

#define I2S_BASE_ADDR    0x40008000UL
#define I2S_FIFO_PA      0x40008008UL   /* physical address of I2S->fifo for DMA */
#define I2S              ((i2s_hw_t *)I2S_BASE_ADDR)
/* I2S_FIFO_DEPTH: number of samples transferred per DMA burst / ISR.
 * Real SoC (INMP441): hardware FIFO depth = 8 (default).
 * Spike simulation: override to SAMPLES_PER_CLIP (8000) via Makefile
 * -DI2S_FIFO_DEPTH=8000 to reduce PIRQ overhead from 1000 bursts to 1 burst.
 * The DMA+interrupt mechanism is fully exercised in both cases. */
#ifndef I2S_FIFO_DEPTH
#define I2S_FIFO_DEPTH   8
#endif

/* Clock divisor: SoC_clock / (sample_rate * 32_bits * 2_channels)
 * 36 MHz FPGA: 36M / (8000 * 32 * 2) = 70
 * 12 MHz sim:  12M / (8000 * 32 * 2) = 23
 * Spike plugin ignores this value but we set it correctly for SoC. */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  70   /* default: FPGA at 36 MHz */
#endif

static void i2s_init(uint32_t clk_div) {
    /* Per i2s_regs.h:
     *   CONF_IRQ_EN  bit 4       (mask 0x10)
     *   CONF_DIV     bits[31:8]  (mask 0xFFFFFF00) */
    I2S->conf = ((clk_div & 0xFFFFFFu) << 8) | (1u << 4);
}

/* ── Interrupt controller note ───────────────────────────────────────────── */
/* KWS-SoC uses Hazard3 with EXTENSION_XH3IRQ=1 (kws_soc.v hardcodes this
 * after the dma_integration merge).  With XH3IRQ enabled, each IRQ must be
 * individually unmasked in the MEIEA CSR (0xbe0) in addition to the standard
 * mie.MEIE + mstatus.MIE enables.  MEIEA resets to 0, so skipping it silently
 * blocks all external interrupts even when MEIE and MIE are both set.
 *
 * kws_soc.v wires irq = {uart_irq[2], i2s_irq[1], dmac_irq[0]}.
 * dmac_irq is IRQ 0 → MEIEA bit 16 (= 16 + irq_index) in group 0.
 *
 * We use dmac_irq (not i2s_irq) as the CPU interrupt.  The DMAC asserts IRQ
 * only after its burst fully completes (FC_REG mode: IRQ when FC_REG==1).
 * By the time the CPU ISR runs, dma_batch is already populated — no sentinel
 * spin needed and no AHB contention with the DMA master write port.
 * (i2s_irq still drives PIRQ[0] to trigger the DMA hardware burst.) */

/* ── RISC-V CSR helpers ──────────────────────────────────────────────────── */

static inline void csr_set_mtvec(void (*handler)(void)) {
    asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)handler));
}
static inline void csr_enable_meie(void) {
    asm volatile ("csrs mie, %0" :: "r"(1u << 11));
}
static inline void csr_enable_mie(void) {
    asm volatile ("csrsi mstatus, 8");
}
static inline void csr_meiea_dmac_en(void) {
    /* Unmask dmac_irq (IRQ 0) in Hazard3 MEIEA — required with XH3IRQ=1 */
    asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16));
}

/* ── MS_DMAC_AHBL (0x6000_0000) ─────────────────────────────────────────── */
/*
 * Register layout from MS_DMAC_AHBL_regs.h:
 *   control @ +0x00, status @ +0x04, saddr @ +0x08, daddr @ +0x0C,
 *   count   @ +0x10, swtrig @ +0x14, fcount @ +0x18
 *
 * CTRL field encoding:
 *   bit  0      EN       — 1 = DMA enabled / armed
 *   bits 11:8   TRIGGER  — 0=SW, 1=PIRQ[0] (i2s_irq)
 *   bits 17:16  STYPE    — 2=word
 *   bits 20:18  SAI      — source auto-increment: 0=none, 4=+4B
 *   bits 25:24  DTYPE    — 2=word
 *   bits 28:26  DAI      — dest auto-increment: 0=none, 4=+4B
 *
 * IRQ (dmac_irq) fires on every completed burst (done pulse latched in ICR).
 * ICR (offset 0x1C) must be cleared by SW (write 1 to bit 0) to de-assert IRQ.
 */
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

/* PIRQ[0]-triggered word transfer: fixed source (FIFO), incrementing dest */
#define DMAC_CTRL_I2S_PIRQ \
    ((1u <<  0) | (1u <<  8) | (2u << 16) | (0u << 18) | (2u << 24) | (4u << 26))

/* Staging buffer: DMA writes raw 32-bit FIFO words here, ISR extracts int8 Q7 */
static volatile uint32_t dma_batch[I2S_FIFO_DEPTH];

static void dma_arm(void) {
    DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u;       /* EN=0 while reconfiguring */
    DMAC->saddr   = I2S_FIFO_PA;
    DMAC->daddr   = (uint32_t)(uintptr_t)dma_batch;
    DMAC->count   = (uint32_t)(I2S_FIFO_DEPTH - 1); /* COUNT = N-1 for N transfers */
    DMAC->control = DMAC_CTRL_I2S_PIRQ;              /* EN=1 — armed on PIRQ[0] */
}

/* ── NNoM static memory buffer ───────────────────────────────────────────── */
/* NNOM_STATIC_BUF_KB: size of the activation scratch buffer.
 * On Spike (unlimited RAM) use 256 KB to be safe.
 * On the real SoC (128 KB total SRAM), reduce after running model_stat()
 * to find the actual peak activation size. */
#ifndef NNOM_STATIC_BUF_KB
#define NNOM_STATIC_BUF_KB 256
#endif
#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t nnom_static_buf[NNOM_STATIC_BUF_KB * 1024];
#endif

/* ── Audio ring buffer ───────────────────────────────────────────────────── */
#define SAMPLES_PER_CLIP  8000     /* 1 second at 8 kHz, int8 Q7 */
#define NUM_CLASSES       11

static volatile int8_t ring_buf[SAMPLES_PER_CLIP];
static volatile int    ring_pos   = 0;
static volatile int    ring_ready = 0;
static volatile uint32_t i2s_irq_count = 0;

/* ── Class names (order matches weights.h CLASSES) ───────────────────────── */
static const char * const class_names[NUM_CLASSES] = {
    "down", "go", "left", "no", "off", "on",
    "right", "stop", "up", "yes", "unknown"
};

/* ── Machine trap handler ────────────────────────────────────────────────── */
/*
 * Flow per dmac_irq interrupt:
 *  1. i2s_irq asserts (FIFO full) → DMA bursts I2S_FIFO_DEPTH words to dma_batch
 *  2. DMA burst completes → dmac_irq fires (FC_REG=1 mode)
 *  3. CPU enters ISR — dma_batch is already fully populated, no spin needed
 *  4. Extract int8 Q7 (upper byte of each 32-bit word) into ring_buf
 *  5a. Ring not full → re-arm DMA for next PIRQ, return
 *  5b. Ring full     → set ring_ready, disable DMA (main re-arms after copy)
 */
void __attribute__((interrupt("machine"))) kws_trap_handler(void) {
    uint32_t mcause, mepc, mtval;
    asm volatile ("csrr %0, mcause" : "=r"(mcause));

    if (!(mcause & 0x80000000u)) {
        /* Exception — print diagnostics and halt */
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

    /* dmac_irq: DMA burst complete — dma_batch is fully populated */
    DMAC->icr = 1u;   /* clear ICR latch so IRQ de-asserts before mret */

    /* Extract int8 Q7: upper byte of each left-justified 32-bit FIFO word */
    for (int i = 0; i < I2S_FIFO_DEPTH; i++) {
        if (ring_pos < SAMPLES_PER_CLIP)
            ring_buf[ring_pos++] = (int8_t)((uint32_t)dma_batch[i] >> 16);
    }

    i2s_irq_count++;

    if (ring_pos >= SAMPLES_PER_CLIP) {
        ring_ready = 1;
        DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u; /* EN=0 — main re-arms after copy */
    } else {
        /* Reset staging buffer pointer; keep EN=1 so DMA re-arms on next PIRQ
         * without an EN=0→EN=1 gap that could drop a sample. */
        DMAC->daddr = (uint32_t)(uintptr_t)dma_batch;
    }
}

/* ── main ────────────────────────────────────────────────────────────────── */

int main(void) {
    uint32_t infer_count = 0;

    uart_init();
    uart_puts("KWS bare-metal firmware (NNoM int8, DMA I2S)\r\n");

    /* ── NNoM init ── */
#ifdef NNOM_USING_STATIC_MEMORY
    nnom_set_static_buf(nnom_static_buf, sizeof(nnom_static_buf));
#endif

    nnom_model_t *model = nnom_model_create();
    if (!model) {
        uart_puts("ERROR: nnom_model_create failed\r\n");
        while (1);
    }

    /* Detect silent model_compile failure: p_data=NULL means nnom_static_buf
     * was too small. model_run() would crash with a store fault otherwise. */
    if (model->head == NULL || model->head->in->tensor->p_data == NULL) {
        uart_puts("ERROR: model_compile failed — nnom_static_buf too small\r\n");
        uart_puts("  head=");
        uart_puthex((uint32_t)(uintptr_t)model->head);
        uart_puts(" p_data=");
        uart_puthex(model->head ? (uint32_t)(uintptr_t)model->head->in->tensor->p_data : 0);
        uart_puts("\r\n");
        while (1);
    }
    uart_puts("Model loaded\r\n");

    /* ── Interrupt + DMA setup ── */
    csr_set_mtvec(kws_trap_handler);
    csr_enable_meie();
    csr_meiea_dmac_en();  /* unmask dmac_irq (IRQ 0) in MEIEA — mandatory with XH3IRQ=1 */

    /* Arm DMA before starting I2S so PIRQ is ready when the first FIFO-full fires */
    dma_arm();

    i2s_init(I2S_CLK_DIV);
    uart_puts("I2S started\r\n");

    csr_enable_mie();

    /* ── Inference loop ── */
    while (1) {
        /* Check ring_ready before wfi — ISRs may have fired before we reach
         * this point; wfi would block forever with ring_ready=1. */
        if (!ring_ready)
            asm volatile ("wfi");

        if (!ring_ready)
            continue;

        uart_puts("CLIP ready samples=");
        uart_putdec(ring_pos);
        uart_puts(" irq=");
        uart_putdec((int)i2s_irq_count);
        uart_puts("\r\n");

        /* Copy ring buffer to NNoM input (8000 int8 Q7 samples) */
        memcpy(nnom_input_data, (const void *)ring_buf,
               (size_t)SAMPLES_PER_CLIP);

        /* Reset ring and re-arm DMA so audio capture overlaps with inference */
        ring_pos   = 0;
        ring_ready = 0;
        dma_arm();

        uart_puts("RUN model #");
        uart_putdec((int)(infer_count + 1));
        uart_puts("\r\n");

        model_run(model);

        uart_puts("RUN done #");
        uart_putdec((int)(infer_count + 1));
        uart_puts("\r\n");
        infer_count++;

        /* Argmax over 11 output scores */
        int    pred      = 0;
        int8_t max_score = nnom_output_data[0];
        for (int j = 1; j < NUM_CLASSES; j++) {
            if (nnom_output_data[j] > max_score) {
                max_score = nnom_output_data[j];
                pred      = j;
            }
        }

        /* Output result: "DETECT:<index>,<name>\r\n" */
        uart_puts("DETECT:");
        uart_putdec(pred);
        uart_putc(',');
        uart_puts(class_names[pred]);
        uart_puts("\r\n");
    }

    return 0;
}
