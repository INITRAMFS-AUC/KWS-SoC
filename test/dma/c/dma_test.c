/*
 * MS_DMAC_AHBL firmware test
 *
 * Tests 1-3: SW-triggered transfers.
 *   Completion is detected by polling SWTRIG — the register auto-clears when
 *   the DMA's internal `done` signal fires.
 *
 * Test 4: PIRQ-triggered DMA — I2S FIFO → SRAM (CPU runs in parallel).
 *   CTRL.TRIGGER[3:0] = 0001 arms the DMA on PIRQ[0] (= i2s_irq).
 *   When the I2S FIFO fills, i2s_irq asserts, the DMA bursts 8 words from the
 *   FIFO into SRAM.  Completion is detected via the DMA IRQ output (dmac_irq,
 *   IRQ 0) which is latched in the ICR register and stays asserted until SW
 *   writes 1 to ICR.done.  The CPU runs an LCG work loop concurrently with
 *   each burst to demonstrate non-blocking operation.
 *
 * COUNT register semantics: COUNT = N-1 for N transfers.
 * Auto-increment encoding: 0=none, 1=+1 byte/step, 2=+2, 4=+4 (word/step).
 *
 * IRQ mapping (kws_soc .irq = {uart_irq, dmac_irq}):
 *   IRQ 0 = dmac_irq  → MEIEA group 0 bit 16
 *   IRQ 1 = uart_irq  → MEIEA group 0 bit 17
 */

#include <stdint.h>
#include "uart.h"
#include "MS_DMAC_AHBL.h"
#include "i2s_regs.h"

#define DMAC_BASE     0x60000000u
#define DMAC_ICR_OFFS 0x1cu   /* ICR register offset — write 1 to bit 0 to clear done IRQ */
#define I2S_BASE      0x40008000u
#define I2S_FIFO_REG  0x40008008u  /* I2S FIFO register address */

/* ---- Test 1: word memcpy ---- */
static volatile uint32_t t1_src[4] = {0xDEADBEEF, 0xCAFEBABE, 0x12345678, 0xABCD1234};
static volatile uint32_t t1_dst[4];

/* ---- Test 2: word fill/scatter (fixed source, incrementing destination) ---- */
static volatile uint32_t t2_src = 0xA5A5A5A5u;
static volatile uint32_t t2_dst[4];

/* ---- Test 3: larger word copy (8 words) ---- */
static volatile uint32_t t3_src[8] = {
    0xA0B0C0D0, 0xE0F01020, 0x30405060, 0x07080900,
    0xAABBCCDD, 0x11223344, 0x55667788, 0x99AABBCC
};
static volatile uint32_t t3_dst[8];

/* ---- Test 4: PIRQ-triggered I2S capture ---- */
#define T4_N_BATCHES 16

static volatile uint32_t t4_capture[8];  /* reused for every batch */
static volatile int      t4_batch_done;

/* ---- RISC-V CSR helpers ---- */
static inline void      csr_set_mtvec(void (*h)(void)) { asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)h)); }
static inline uintptr_t csr_get_mtvec(void)            { uintptr_t v; asm volatile ("csrr %0, mtvec" : "=r"(v)); return v; }
static inline void      csr_meie_en(void)              { asm volatile ("csrs mie, %0"    :: "r"(1u << 11)); }
static inline void      csr_meie_dis(void)             { asm volatile ("csrc mie, %0"    :: "r"(1u << 11)); }
static inline void      csr_mie_en(void)               { asm volatile ("csrsi mstatus, 8"); }
static inline void      csr_mie_dis(void)              { asm volatile ("csrci mstatus, 8"); }

/*
 * Hazard3 MEIEA CSR (0xbe0): per-IRQ enable array (requires EXTENSION_XH3IRQ=1).
 * IRQs grouped 16 per write: wdata_raw[4:0]=group index, wdata[16+(irq%16)]=enable.
 * IRQ 0 = dmac_irq → bit 16 of group 0.
 */
static inline void csr_meiea_dmac_en(void)  { asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 16)); }
static inline void csr_meiea_dmac_dis(void) { asm volatile ("csrw 0xbe0, %0" :: "r"(0u)); }

/* ---- helpers ---- */

static int all_pass = 1;

static void check(int cond, const char *label)
{
    if (!cond) {
        all_pass = 0;
        uart_printf("  %s FAIL\r\n", label);
    } else {
        uart_printf("  %s OK\r\n", label);
    }
}

/*
 * Configure the DMAC for a word-to-word transfer and fire it.
 * Blocks until SWTRIG auto-clears (transfer complete).
 *
 * sai / dai: auto-increment step — 0=none, 1=+1B, 2=+2B, 4=+4B (word step)
 */
static void dma_word_transfer(uint32_t src, uint32_t dst, uint16_t count,
                               uint8_t sai, uint8_t dai)
{
    uint32_t ctrl = (1u  <<  0)
                  | (0u  <<  8)
                  | (2u  << 16)
                  | ((uint32_t)sai << 18)
                  | (2u  << 24)
                  | ((uint32_t)dai << 26);

    MS_DMAC_setControlReg(DMAC_BASE, (int)ctrl);
    MS_DMAC_setSourceAddr(DMAC_BASE, (int)src);
    MS_DMAC_setDestinationAddr(DMAC_BASE, (int)dst);
    MS_DMAC_setCount(DMAC_BASE, count);

    MS_DMAC_setSWTrigger(DMAC_BASE, 1);
    while (MS_DMAC_getSWTrigger(DMAC_BASE))
        ;
}

/* ---- Test implementations ---- */

static void test1_word_memcpy(void)
{
    uart_puts("[Test 1] Word memcpy (4 words, SAI=+4 DAI=+4)\r\n");

    for (int i = 0; i < 4; i++) t1_dst[i] = 0;
    dma_word_transfer((uint32_t)t1_src, (uint32_t)t1_dst, 3, 4, 4);

    int pass = 1;
    for (int i = 0; i < 4; i++) {
        int ok = (t1_dst[i] == t1_src[i]);
        uart_printf("  [%x] src=%x dst=%x %s\r\n",
                    (uint32_t)i, t1_src[i], t1_dst[i], ok ? "OK" : "FAIL");
        pass &= ok;
    }
    all_pass &= pass;
    uart_printf("  --> %s\r\n\r\n", pass ? "PASS" : "FAIL");
}

static void test2_word_fill(void)
{
    uart_puts("[Test 2] Word fill/scatter (SAI=0 fixed src, DAI=+4)\r\n");
    uart_printf("  source word: %x\r\n", t2_src);

    for (int i = 0; i < 4; i++) t2_dst[i] = 0;
    dma_word_transfer((uint32_t)&t2_src, (uint32_t)t2_dst, 3, 0, 4);

    int pass = 1;
    for (int i = 0; i < 4; i++) {
        int ok = (t2_dst[i] == t2_src);
        uart_printf("  [%x] dst=%x %s\r\n", (uint32_t)i, t2_dst[i], ok ? "OK" : "FAIL");
        pass &= ok;
    }
    all_pass &= pass;
    uart_printf("  --> %s\r\n\r\n", pass ? "PASS" : "FAIL");
}

static void test3_large_copy(void)
{
    uart_puts("[Test 3] Word copy - 8 words (SAI=+4 DAI=+4)\r\n");

    for (int i = 0; i < 8; i++) t3_dst[i] = 0;
    dma_word_transfer((uint32_t)t3_src, (uint32_t)t3_dst, 7, 4, 4);

    int pass = 1;
    for (int i = 0; i < 8; i++) {
        int ok = (t3_dst[i] == t3_src[i]);
        uart_printf("  [%x] src=%x dst=%x %s\r\n",
                    (uint32_t)i, t3_src[i], t3_dst[i], ok ? "OK" : "FAIL");
        pass &= ok;
    }
    all_pass &= pass;
    uart_printf("  --> %s\r\n\r\n", pass ? "PASS" : "FAIL");
}

/* ---- Test 4: PIRQ-triggered DMA — I2S FIFO → SRAM ---- */

/*
 * ISR for dmac_irq (IRQ 0).
 * Clear ICR before disabling EN so the IRQ line de-asserts before mret
 * restores MIE, preventing immediate re-entry.
 */
static void __attribute__((interrupt("machine"))) dmac_isr(void)
{
    *(volatile uint32_t *)(DMAC_BASE + DMAC_ICR_OFFS) = 1u;
    MS_DMAC_enable(DMAC_BASE, 0);
    t4_batch_done = 1;
}

static void test4_pirq_i2s_dma(void)
{
    uart_puts("[Test 4] PIRQ-triggered DMA: I2S FIFO \xe2\x86\x92 SRAM (CPU works in parallel)\r\n");
    uart_printf("  Capturing 0x%x batches x 8 words = 0x%x samples\r\n",
                (uint32_t)T4_N_BATCHES, (uint32_t)(T4_N_BATCHES * 8));
    uart_puts("  (Compare hex values against sim/debug_audio.hex)\r\n\r\n");

    /*
     * CTRL for PIRQ mode:
     *   EN=1         bit  0
     *   TRIGGER=0001 bits 11:8  → armed on PIRQ[0] (i2s_irq)
     *   STYPE=2      bits 17:16 → word reads from FIFO
     *   SAI=0        bits 20:18 → fixed source (FIFO register, no increment)
     *   DTYPE=2      bits 25:24 → word writes to SRAM
     *   DAI=4        bits 28:26 → +4 B per step (word increment)
     */
    uint32_t ctrl_pirq = (1u  <<  0)
                       | (1u  <<  8)
                       | (2u  << 16)
                       | (0u  << 18)
                       | (2u  << 24)
                       | (4u  << 26);

    /* Configure I2S */
    volatile i2s_hw_t *i2s = (volatile i2s_hw_t *)I2S_BASE;
    i2s->conf = (4u << I2S_CONF_DIV_LSB) | (1u << I2S_CONF_IRQ_EN_LSB);

    /* Install ISR; enable dmac_irq (IRQ 0: MEIEA group 0 bit 16) */
    uintptr_t saved_mtvec = csr_get_mtvec();
    csr_set_mtvec(dmac_isr);
    csr_meiea_dmac_en();
    csr_meie_en();

    /* One-time DMA configuration — source, dest, count stay fixed across batches */
    MS_DMAC_setControlReg(DMAC_BASE, (int)(ctrl_pirq & ~1u)); /* EN=0 */
    MS_DMAC_setSourceAddr(DMAC_BASE, (int)I2S_FIFO_REG);
    MS_DMAC_setDestinationAddr(DMAC_BASE, (int)t4_capture);
    MS_DMAC_setCount(DMAC_BASE, 7); /* COUNT=7 → 8 transfers */

    int      pass            = 1;
    uint32_t total_cpu_iters = 0;
    uint32_t cpu_state       = 0xDEADBEEFu;

    for (int batch = 0; batch < T4_N_BATCHES; batch++) {

        /*
         * MIE=0 during re-arm: prevents the ISR firing on a stale i2s_irq
         * while sentinels are being written but the DMA is not yet armed.
         */
        for (int i = 0; i < 8; i++) t4_capture[i] = 0x5A5A5A5Au;
        t4_batch_done = 0;

        MS_DMAC_enable(DMAC_BASE, 1); /* EN=1 → DMA now armed on PIRQ */
        csr_mie_en();

        /*
         * CPU work loop — runs concurrently with the DMA burst.
         * The ISR fires between LCG iterations once `done` latches into ICR.
         */
        uint32_t batch_iters = 0;
        while (!t4_batch_done) {
            cpu_state = cpu_state * 1664525u + 1013904223u;
            batch_iters++;
        }

        csr_mie_dis();
        total_cpu_iters += batch_iters;

        if (t4_capture[7] == 0x5A5A5A5Au) {
            uart_printf("  Batch 0x%x: INCOMPLETE (DMA did not fill capture buffer)\r\n",
                        (uint32_t)batch);
            pass = 0;
            break;
        }

        uart_printf("  Batch 0x%x (samples 0x%x-0x%x)  cpu_iters=0x%x:\r\n",
                    (uint32_t)batch,
                    (uint32_t)(batch * 8),
                    (uint32_t)(batch * 8 + 7),
                    batch_iters);
        for (int i = 0; i < 8; i++)
            uart_printf("    [0x%x] 0x%x\r\n",
                        (uint32_t)(batch * 8 + i), t4_capture[i]);
    }

    uart_printf("  Total CPU work: 0x%x LCG iters, final state=0x%x\r\n",
                total_cpu_iters, cpu_state);

    csr_meiea_dmac_dis();
    csr_meie_dis();
    csr_set_mtvec((void (*)(void))saved_mtvec);
    i2s->conf = 0; /* stop I2S */

    all_pass &= pass;
    uart_printf("  --> %s\r\n\r\n", pass ? "PASS" : "FAIL");
}

int main(void)
{
    uart_init();
    uart_puts("=== MS_DMAC_AHBL Test Suite ===\r\n\r\n");

    test1_word_memcpy();
    test2_word_fill();
    test3_large_copy();
    test4_pirq_i2s_dma();

    uart_printf("=== All 4 tests %s ===\r\n",
                all_pass ? "PASSED" : "FAILED");

    while (1)
        ;
    return 0;
}
