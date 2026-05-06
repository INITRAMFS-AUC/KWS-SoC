/*
 * recordI2s.c — Lossless I2S recording firmware
 *
 * Autonomous DMA drain: the I2S half-full IRQ is wired to DMAC.PIRQ[0], so
 * each i2s_irq fires the DMA burst directly without CPU involvement.  The CPU
 * only handles dmac_irq at the end of each burst to flip the ring-buffer half
 * and never touches the data path inside the ISR (no UART in ISR).  The main
 * loop drains completed halves over UART asynchronously — UART latency cannot
 * stall the FIFO drain.
 *
 * I2S is configured with DS_EN=0 and WIDTH=32 (no downsampling, no packing) by
 * default.  WIDTH can be overridden at build time:
 *   -DI2S_WIDTH_MODE=I2S_CONF_WIDTH_32  (default; 1 sample/word)
 *   -DI2S_WIDTH_MODE=I2S_CONF_WIDTH_16  (2 samples/word, audio[23:8])
 *   -DI2S_WIDTH_MODE=I2S_CONF_WIDTH_8   (4 samples/word, audio[23:16])
 *
 * Output: one "XXXXXXXX\r\n" line per FIFO word — diff-able against
 * sim/playback_samples.hex (after the receiver's >> 7 transform).
 *
 * IRQ map (kws_soc.v): IRQ 0 = dmac_irq → MEIEA bit 16.
 */

#include <stdint.h>
#include "uart.h"
#include "MS_DMAC_AHBL.h"
#include "i2s_regs.h"

#define I2S_BASE_ADDR  0x40008000u
#define I2S_FIFO_REG   0x40008008u
#define I2S            ((volatile i2s_hw_t *)I2S_BASE_ADDR)

#define DMAC_BASE      0x60000000u
#define DMAC_ICR_OFFS  0x1cu

#ifndef I2S_DMA_BURST_WORDS
#error "I2S_DMA_BURST_WORDS must be defined by the Makefile"
#endif

#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  17
#endif

#ifndef RECORD_N_SAMPLES
#define RECORD_N_SAMPLES  8000
#endif

#ifndef I2S_WIDTH_MODE
#define I2S_WIDTH_MODE I2S_CONF_WIDTH_32
#endif

/* RECORD_N_SAMPLES must be a multiple of I2S_DMA_BURST_WORDS (each PIRQ burst
 * transfers BURST_WORDS at a time).  Large contiguous buffer in SRAM. */
#if (RECORD_N_SAMPLES % I2S_DMA_BURST_WORDS) != 0
#error "RECORD_N_SAMPLES must be a multiple of I2S_DMA_BURST_WORDS"
#endif
#define N_BURSTS (RECORD_N_SAMPLES / I2S_DMA_BURST_WORDS)

static volatile uint32_t buffer[RECORD_N_SAMPLES];

static volatile uint32_t bursts_done = 0; /* incremented by ISR */
static volatile int      capture_done = 0;

/* ---- CSR helpers ---- */
static inline void csr_set_mtvec(void (*h)(void)) { asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)h)); }
static inline void csr_meie_en(void)              { asm volatile ("csrs mie, %0" :: "r"(1u << 11)); }
static inline void csr_meie_dis(void)             { asm volatile ("csrc mie, %0" :: "r"(1u << 11)); }
static inline void csr_mie_en(void)               { asm volatile ("csrsi mstatus, 8"); }
static inline void csr_mie_dis(void)              { asm volatile ("csrci mstatus, 8"); }
/* Enable only dmac_irq (bit 16); i2s_irq routes to DMAC.PIRQ[0], not the CPU. */
static inline void csr_meiea_en(void)             { asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16)); }
static inline void csr_meiea_dis(void)            { asm volatile ("csrw 0xbe0, %0" :: "r"(0u)); }

static void print_hex32(uint32_t v)
{
    static const char h[] = "0123456789ABCDEF";
    for (int s = 28; s >= 0; s -= 4)
        uart_putc(h[(v >> s) & 0xF]);
    uart_putc('\r');
    uart_putc('\n');
}

/*
 * dmac_irq ISR — minimal.  Clear ICR, advance DADDR to the next slot in the
 * capture buffer, bump bursts_done.  No UART, no buffer scan.  When the
 * buffer is full, disable EN and signal main to stop.
 */
static void __attribute__((interrupt("machine"))) record_isr(void)
{
    *(volatile uint32_t *)(DMAC_BASE + DMAC_ICR_OFFS) = 1u;

    uint32_t next = bursts_done + 1u;
    bursts_done = next;

    if (next >= N_BURSTS) {
        MS_DMAC_enable(DMAC_BASE, 0);
        capture_done = 1;
    } else {
        MS_DMAC_setDestinationAddr(DMAC_BASE,
            (int)(uintptr_t)&buffer[next * I2S_DMA_BURST_WORDS]);
    }
}

int main(void)
{
    uart_init();
    uart_printf("# recordI2s  clk_div=%x  n=%x  burst=%x  width=%x\r\n",
                (uint32_t)I2S_CLK_DIV,
                (uint32_t)RECORD_N_SAMPLES,
                (uint32_t)I2S_DMA_BURST_WORDS,
                (uint32_t)I2S_WIDTH_MODE);

    /*
     * DMAC CTRL — PIRQ-triggered (TRIGGER=4'b0001 → fires on PIRQ[0]=i2s_irq):
     *   EN=1            bit 0
     *   TRIGGER=0001    bits 11:8  → autonomous DMA on i2s_irq
     *   STYPE=2         bits 17:16 → word reads from I2S FIFO
     *   SAI=0           bits 20:18 → fixed source (FIFO register)
     *   DTYPE=2         bits 25:24 → word writes
     *   DAI=4           bits 28:26 → +4 B per step
     */
    uint32_t ctrl_pirq = (1u  <<  0)
                       | (1u  <<  8)
                       | (2u  << 16)
                       | (0u  << 18)
                       | (2u  << 24)
                       | (4u  << 26);

    /* Configure I2S: IRQ_EN=1, DS_EN=0, WIDTH=I2S_WIDTH_MODE.
     * IRQ_EN gates the i2s_irq line — same line goes both to the CPU IRQ map
     * and to DMAC.PIRQ[0]; we only enable DMAC.PIRQ here. */
    I2S->conf =
          ((I2S_CLK_DIV    << I2S_CONF_DIV_LSB)   & I2S_CONF_DIV_MASK)
        | I2S_CONF_IRQ_EN_MASK
        | (((uint32_t)I2S_WIDTH_MODE << I2S_CONF_WIDTH_LSB) & I2S_CONF_WIDTH_MASK);

    /* DMA setup — EN=0 while writing fields */
    MS_DMAC_setControlReg(DMAC_BASE, (int)(ctrl_pirq & ~1u));
    MS_DMAC_setSourceAddr(DMAC_BASE, (int)I2S_FIFO_REG);
    MS_DMAC_setDestinationAddr(DMAC_BASE, (int)(uintptr_t)&buffer[0]);
    MS_DMAC_setCount(DMAC_BASE, I2S_DMA_BURST_WORDS - 1);

    /* Pre-clear any leftover icr_done from an earlier test, then arm IRQs. */
    *(volatile uint32_t *)(DMAC_BASE + DMAC_ICR_OFFS) = 1u;
    csr_set_mtvec(record_isr);
    csr_meiea_en();
    csr_meie_en();

    /* Enable DMA last so PIRQ-triggered bursts only start after the buffer
     * is armed and the ISR is installed.  From here on the CPU is idle —
     * the DMA captures every sample autonomously into `buffer`. */
    MS_DMAC_enable(DMAC_BASE, 1);
    csr_mie_en();

    while (!capture_done)
        asm volatile ("wfi");

    csr_mie_dis();
    csr_meiea_dis();
    csr_meie_dis();
    I2S->conf = 0;

    /* Capture finished — now print all RECORD_N_SAMPLES words.  The CPU was
     * idle during capture, so no samples were dropped. */
    for (uint32_t i = 0; i < (uint32_t)RECORD_N_SAMPLES; i++)
        print_hex32(buffer[i]);

    uart_printf("# DONE bursts=%x samples=%x\r\n",
                bursts_done, (uint32_t)RECORD_N_SAMPLES);
    return 0;
}
