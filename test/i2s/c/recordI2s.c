/*
 * recordI2s.c — Raw I2S recording firmware
 *
 * Configures I2S with no downsampling (DS_EN=0) and no hardware quantization
 * (Q8_EN=0).  Each FIFO word = {8'b0, audio[23:0]}: one 24-bit sample.
 *
 * Flow: I2S FIFO half-full → i2s_irq (CPU IRQ 2) → swtrig=1 → DMA burst
 *       (I2S_DMA_BURST_WORDS words) → dmac_irq (CPU IRQ 0) → ISR prints buffer.
 *
 * Output: one "XXXXXXXX\r\n" line per FIFO word — the raw 32-bit value as
 * read from the I2S FIFO, suitable for diff against sim/playback_samples.hex.
 *
 * IRQ mapping (kws_soc.v):
 *   IRQ 0 = dmac_irq → Hazard3 MEIEA group 0 bit 16
 *   IRQ 2 = i2s_irq  → Hazard3 MEIEA group 0 bit 18
 */

#include <stdint.h>
#include "uart.h"
#include "MS_DMAC_AHBL.h"
#include "i2s_regs.h"

#define I2S_BASE_ADDR  0x40008000u
#define I2S_FIFO_REG   0x40008008u   /* address of I2S->fifo, DMA source */
#define I2S            ((volatile i2s_hw_t *)I2S_BASE_ADDR)

#define DMAC_BASE      0x60000000u
#define DMAC_ICR_OFFS  0x1cu         /* write 1 to bit 0 to clear dmac_irq */

#ifndef I2S_DMA_BURST_WORDS
#error "I2S_DMA_BURST_WORDS must be defined by the Makefile"
#endif

/* Clock divider (no DS_EN, so this is the true sample rate):
 *   36 MHz FPGA: cfg_div=17 → 15625 Hz  |  cfg_div=35 → 7812.5 Hz
 * Override: make record-i2s I2S_CLK_DIV=35 */
#ifndef I2S_CLK_DIV
#define I2S_CLK_DIV  17
#endif

/* Number of 16-bit samples to record, then stop.
 * Set to 0 (-DRECORD_N_SAMPLES=0) to stream indefinitely. */
#ifndef RECORD_N_SAMPLES
#define RECORD_N_SAMPLES  8000
#endif

/* DMA burst buffer — holds one batch of raw 32-bit FIFO words. */
static volatile uint32_t capture[I2S_DMA_BURST_WORDS];

static volatile uint32_t sample_count = 0;
static volatile int      done         = 0;

/* ---- CSR helpers ---- */
static inline void csr_set_mtvec(void (*h)(void)) { asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)h)); }
static inline void csr_meie_en(void)               { asm volatile ("csrs mie, %0"    :: "r"(1u << 11)); }
static inline void csr_meie_dis(void)              { asm volatile ("csrc mie, %0"    :: "r"(1u << 11)); }
static inline void csr_mie_en(void)                { asm volatile ("csrsi mstatus, 8"); }
static inline void csr_mie_dis(void)               { asm volatile ("csrci mstatus, 8"); }
static inline uint32_t csr_meipa_read(void)        { uint32_t v; asm volatile ("csrr %0, 0xbe1" : "=r"(v)); return v; }
/* Enable dmac_irq (bit 16) + i2s_irq (bit 18) — single write replaces group mask. */
static inline void csr_meiea_en(void)  { asm volatile ("csrw 0xbe0, %0" :: "r"((1u << 16) | (1u << 18))); }
static inline void csr_meiea_dis(void) { asm volatile ("csrw 0xbe0, %0" :: "r"(0u)); }

/* Print exactly 8 hex digits + CRLF (matches sim/playback_samples.hex format). */
static void print_hex32(uint32_t v)
{
    static const char h[] = "0123456789ABCDEF";
    for (int s = 28; s >= 0; s -= 4)
        uart_putc(h[(v >> s) & 0xF]);
    uart_putc('\r');
    uart_putc('\n');
}

/*
 * Combined ISR for dmac_irq (bit 16) and i2s_irq (bit 18).
 *   i2s_irq:  FIFO half-full — fire one SW burst, then mask i2s_irq.
 *   dmac_irq: burst done — clear ICR, process buffer, reset DADDR, re-enable i2s_irq.
 *
 * i2s_irq is level-sensitive: masking it after swtrig prevents the CPU from
 * spinning in the ISR and starving the DMA of AHB access while a burst is
 * in flight. The dmac handler re-enables it once the burst completes.
 * dmac is handled before i2s so DADDR is updated before the next swtrig.
 */
static void __attribute__((interrupt("machine"))) record_isr(void)
{
    uint32_t meipa = csr_meipa_read();

    if (meipa & (1u << 16)) {
        /* dmac_irq: burst complete — clear ICR, process buffer, re-arm */
        *(volatile uint32_t *)(DMAC_BASE + DMAC_ICR_OFFS) = 1u;

        for (int i = 0; i < I2S_DMA_BURST_WORDS; i++) {
            if (RECORD_N_SAMPLES && sample_count >= (uint32_t)RECORD_N_SAMPLES) {
                MS_DMAC_enable(DMAC_BASE, 0);
                done = 1;
                return;
            }
            /* Raw FIFO word: {8'b0, audio[23:0]} — print verbatim. */
            print_hex32(capture[i]);
            sample_count++;
        }

        /* Reset DADDR to the same buffer slot for next burst */
        MS_DMAC_setDestinationAddr(DMAC_BASE, (int)(uintptr_t)capture);
        /* Re-enable i2s_irq (bit 18) now that the burst is complete */
        csr_meiea_en();
    }

    if (meipa & (1u << 18)) {
        /* i2s_irq: FIFO half-full — fire one burst, mask i2s_irq until burst done */
        if (!done) {
            MS_DMAC_setSWTrigger(DMAC_BASE, 1);
            /* Keep only dmac_irq enabled while burst is in flight */
            asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16));
        }
    }
}

int main(void)
{
    uart_init();
    uart_printf("# recordI2s  clk_div=%u  n=%u  burst=%u\r\n",
                (uint32_t)I2S_CLK_DIV,
                (uint32_t)RECORD_N_SAMPLES,
                (uint32_t)I2S_DMA_BURST_WORDS);

    /*
     * DMAC CTRL for SW trigger mode:
     *   EN=1         bit  0
     *   TRIGGER=0000 bits 11:8  → SW trigger (swtrig=1 from i2s_irq ISR)
     *   STYPE=2      bits 17:16 → 32-bit word reads
     *   SAI=0        bits 20:18 → fixed source (FIFO, no increment)
     *   DTYPE=2      bits 25:24 → 32-bit word writes
     *   DAI=4        bits 28:26 → +4 B per step (word increment into buffer)
     */
    uint32_t ctrl_sw = (1u << 0)
                     | (0u << 8)
                     | (2u << 16)
                     | (0u << 18)
                     | (2u << 24)
                     | (4u << 26);

    /* Configure I2S: IRQ_EN only — no DS_EN, no Q8_EN.
     * i2s_irq fires when FIFO reaches half-full (I2S_DMA_BURST_WORDS words). */
    I2S->conf = ((I2S_CLK_DIV << I2S_CONF_DIV_LSB) & I2S_CONF_DIV_MASK)
              | I2S_CONF_IRQ_EN_MASK;

    /* Install combined ISR; enable dmac_irq (bit 16) + i2s_irq (bit 18) */
    csr_set_mtvec(record_isr);
    csr_meiea_en();
    csr_meie_en();

    /* Arm DMA — EN=1, TRIGGER=0; each i2s_irq ISR fires one swtrig burst. */
    MS_DMAC_setControlReg(DMAC_BASE, (int)(ctrl_sw & ~1u)); /* EN=0 while configuring */
    MS_DMAC_setSourceAddr(DMAC_BASE, (int)I2S_FIFO_REG);
    MS_DMAC_setDestinationAddr(DMAC_BASE, (int)(uintptr_t)capture);
    MS_DMAC_setCount(DMAC_BASE, I2S_DMA_BURST_WORDS - 1);  /* COUNT = N-1 for N transfers */
    MS_DMAC_enable(DMAC_BASE, 1);
    csr_mie_en();

    while (!done)
        asm volatile ("wfi");

    csr_mie_dis();
    csr_meiea_dis();
    csr_meie_dis();
    I2S->conf = 0;

    uart_printf("# DONE %u samples\r\n", sample_count);
    return 0;
}
