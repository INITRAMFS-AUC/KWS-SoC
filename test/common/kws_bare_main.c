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
 *     -DUSE_MCYCLE_CSR             enable in-firmware cycle measurement +
 *                                  "CYCLES:" UART print.  Off by default —
 *                                  the Verilator/CXXRTL VPI testbench
 *                                  already records accurate cycle counts.
 *     -DKWS_DEBUG_DUMP_FIRST_CLIP  emit an `ebreak` after the first full
 *                                  ring_buf is captured, before memcpy /
 *                                  inference.  Used together with
 *                                  scripts/dump_ring_buf.gdb to dump the
 *                                  captured Q7 samples and diff them
 *                                  against the audio hex file fed by
 *                                  i2s_mic_sim.  Off by default.
 *
 * PIPELINE
 * --------
 *  i2s_irq (FIFO full)  →  hardware DMA burst  →  dmac_irq (FC_REG=1)  →
 *  ISR copies dma_batch into ring_buf, re-arms; main wakes via WFI when
 *  ring is full, runs inference, prints "DETECT:" / "CYCLES:" / "IRQS:".
 *
 * REGISTER MAP (KWS-SoC)
 *  UART  0x40004000   uart_mini (115200 8N1, CLK_MHZ FPGA / sim)
 *  I2S   0x40008000   apb_i2s_receiver (INMP441 MEMS mic, 8 kHz mono)
 *  DMAC  0x60000000   MS_DMAC_AHBL (PIRQ[0]=i2s_irq)
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
    volatile uint32_t id;    /* 0x00: peripheral ID (ROV = 0xDEADCAFE) */
    volatile uint32_t conf;  /* 0x04: [31:8]=clk_div, [4]=irq_en      */
    volatile uint32_t fifo;  /* 0x08: audio FIFO (read 32-bit sample) */
} i2s_hw_t;

#define I2S_BASE_ADDR    0x40008000UL
#define I2S_FIFO_PA      0x40008008UL   /* physical address of I2S->fifo for DMA */
#define I2S              ((i2s_hw_t *)I2S_BASE_ADDR)

#ifndef I2S_FIFO_DEPTH
#define I2S_FIFO_DEPTH   8
#endif

/* I2S clock divisor: SoC_clock / (sample_rate * 32_bits * 2_channels)
 *   36 MHz FPGA: 36e6 / (8000 * 32 * 2) = 70  → 8 kHz mono int8 Q7.
 * Override on the make line (e.g. -DI2S_CLK_DIV=23 for 12 MHz sim). */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  70
#endif

static void i2s_init(uint32_t clk_div) {
    /* CONF_IRQ_EN bit 4 (0x10), CONF_DIV bits[31:8]. */
    I2S->conf = ((clk_div & 0xFFFFFFu) << 8) | (1u << 4);
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

/* PIRQ[0]-triggered word transfer: fixed source (FIFO), incrementing dest.
 *   bit 0      EN=1
 *   bits 11:8  TRIGGER = 1 (PIRQ[0]=i2s_irq)
 *   bits 17:16 STYPE   = 2 (word)
 *   bits 20:18 SAI     = 0 (no source increment — FIFO)
 *   bits 25:24 DTYPE   = 2 (word)
 *   bits 28:26 DAI     = 4 (+4B per word) */
#define DMAC_CTRL_I2S_PIRQ \
    ((1u <<  0) | (1u <<  8) | (2u << 16) | (0u << 18) | (2u << 24) | (4u << 26))

static volatile uint32_t dma_batch[I2S_FIFO_DEPTH];

static void dma_arm(void) {
    DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u;       /* EN=0 while reconfiguring */
    DMAC->saddr   = I2S_FIFO_PA;
    DMAC->daddr   = (uint32_t)(uintptr_t)dma_batch;
    DMAC->count   = (uint32_t)(I2S_FIFO_DEPTH - 1); /* COUNT = N-1 for N transfers */
    DMAC->control = DMAC_CTRL_I2S_PIRQ;              /* EN=1 — armed on PIRQ[0] */
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
static inline void csr_meiea_dmac_en(void) {
    /* Unmask dmac_irq (IRQ 0) in Hazard3 MEIEA — required with XH3IRQ=1.
     * kws_soc.v wires irq = {uart_irq[2], i2s_irq[1], dmac_irq[0]}.
     * IRQ 0 → MEIEA group 0 bit 16. */
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
#ifdef NNOM_USING_STATIC_MEMORY
static uint8_t nnom_static_buf[NNOM_STATIC_BUF_KB * 1024];
#endif

/* ── Audio ring buffer ───────────────────────────────────────────────────── */
#define SAMPLES_PER_CLIP  8000     /* 1 second at 8 kHz, int8 Q7 */
#define NUM_CLASSES       11

static volatile int8_t  ring_buf[SAMPLES_PER_CLIP];
static volatile int     ring_pos      = 0;
static volatile int     ring_ready    = 0;
static volatile int     i2s_irq_count = 0;

/* ── Class names (order must match the weights header) ───────────────────── */
static const char * const class_names[NUM_CLASSES] = {
    "down", "go", "left", "no", "off", "on",
    "right", "stop", "up", "yes", "unknown"
};

/* ── Machine trap handler ────────────────────────────────────────────────── */
/*
 *  1. i2s_irq asserts (FIFO full) → DMA hardware burst into dma_batch.
 *  2. DMA burst completes (FC_REG=1) → dmac_irq fires.
 *  3. ISR copies int8 Q7 (upper byte of each FIFO word) into ring_buf.
 *  4. Ring not full: keep EN=1, just reset DADDR — avoids dropped samples.
 *  5. Ring full    : disable DMA, signal main; main re-arms after copy.
 */
void __attribute__((interrupt("machine"), aligned(4))) kws_trap_handler(void) {
    uint32_t mcause, mepc, mtval;
    asm volatile ("csrr %0, mcause" : "=r"(mcause));

    if (!(mcause & 0x80000000u)) {
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

    DMAC->icr = 1u;   /* clear ICR latch so IRQ de-asserts before mret */

    for (int i = 0; i < I2S_FIFO_DEPTH; i++) {
        if (ring_pos < SAMPLES_PER_CLIP)
            ring_buf[ring_pos++] = (int8_t)((uint32_t)dma_batch[i] >> 16);
    }
    i2s_irq_count++;

    if (ring_pos >= SAMPLES_PER_CLIP) {
        ring_ready = 1;
        DMAC->control = DMAC_CTRL_I2S_PIRQ & ~1u; /* EN=0 — main re-arms after copy */
    } else {
        DMAC->daddr = (uint32_t)(uintptr_t)dma_batch;
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
    csr_meiea_dmac_en();
    dma_arm();
    i2s_init(I2S_CLK_DIV);
    uart_puts("I2S started\r\n");
    csr_enable_mie();

#ifdef KWS_DEBUG_DUMP_FIRST_CLIP
    int dumped = 0;
#endif

    while (1) {
        if (!ring_ready)
            asm volatile ("wfi");
        if (!ring_ready)
            continue;

#ifdef KWS_DEBUG_DUMP_FIRST_CLIP
        /* Halt after the FIRST captured clip so GDB can read ring_buf and
         * diff it against the audio hex fed by i2s_mic_sim.  See
         * scripts/dump_ring_buf.gdb. */
        if (!dumped) {
            dumped = 1;
            asm volatile ("ebreak");
        }
#endif

        memcpy(nnom_input_data, (const void *)ring_buf,
               (size_t)SAMPLES_PER_CLIP);

        ring_pos   = 0;
        ring_ready = 0;
        dma_arm();

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

        uart_puts("IRQS:");
        uart_putdec(i2s_irq_count);
        uart_puts("\r\n");
        uart_puts("DETECT:");
        uart_putdec(pred);
        uart_putc(',');
        uart_puts(class_names[pred]);
        uart_puts("\r\n");
#ifdef USE_MCYCLE_CSR
        uart_puts("CYCLES:");
        uart_putdec((int)cycles);
        uart_puts("\r\n");
#endif
    }

    return 0;
}
