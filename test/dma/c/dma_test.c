/*
 * MS_DMAC_AHBL firmware test
 *
 * Four tests exercising the DMA controller at 0x6000_0000.
 *
 * Tests 1-3: SW-triggered transfers.
 *   Completion is detected by polling SWTRIG — the register auto-clears when
 *   the DMA's internal `done` signal fires, which is more reliable than polling
 *   STATUS.DONE (a 1-cycle combinatorial pulse for multi-word transfers).
 *
 * Test 4: PIRQ-triggered transfer (I2S FIFO → SRAM).
 *   CTRL.TRIGGER[3:0] = 0001 arms the DMA on PIRQ[0] (= i2s_irq).
 *   When the I2S FIFO fills, i2s_irq asserts, the DMA bursts 8 words from the
 *   FIFO into SRAM, the FIFO empties, and i2s_irq deasserts.
 *   Completion is detected by polling the last destination word for a change
 *   from its sentinel value (STATUS.DONE is a 1-cycle pulse in PIRQ mode too).
 *
 * COUNT register semantics: COUNT = N-1 for N transfers.
 * Auto-increment encoding: 0=none, 1=+1 byte/step, 2=+2, 4=+4 (word/step).
 */

#include <stdint.h>
#include "uart.h"
#include "MS_DMAC_AHBL.h"
#include "i2s_regs.h"

#define DMAC_BASE    0x60000000u
#define I2S_BASE     0x40008000u
#define I2S_FIFO_REG 0x40008008u  /* I2S FIFO register address */

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

/* ---- RISC-V CSR helpers (used by test 4) ---- */
static inline void      csr_set_mtvec(void (*h)(void)) { asm volatile ("csrw mtvec, %0" :: "r"((uintptr_t)h)); }
static inline uintptr_t csr_get_mtvec(void)            { uintptr_t v; asm volatile ("csrr %0, mtvec" : "=r"(v)); return v; }
static inline void      csr_meie_en(void)              { asm volatile ("csrs mie, %0"    :: "r"(1u << 11)); }
static inline void      csr_meie_dis(void)             { asm volatile ("csrc mie, %0"    :: "r"(1u << 11)); }
static inline void      csr_mie_en(void)               { asm volatile ("csrsi mstatus, 8"); }
static inline void      csr_mie_dis(void)              { asm volatile ("csrci mstatus, 8"); }
static inline void      wfi(void)                      { asm volatile ("wfi"); }
/*
 * Hazard3 MEIEA CSR (0xbe0): per-IRQ enable array (requires EXTENSION_XH3IRQ=1).
 * IRQs grouped 16 per write: wdata_raw[4:0]=group index, wdata[16+(irq%16)]=enable.
 * irq={uart_irq[2], i2s_irq[1], dmac_irq[0]} → i2s_irq is IRQ 1, bit 17 of group 0.
 */
static inline void csr_meiea_i2s_en(void)  { asm volatile ("csrw 0xbe0, %0" :: "r"(1u << 17)); }
static inline void csr_meiea_i2s_dis(void) { asm volatile ("csrw 0xbe0, %0" :: "r"(0u)); }

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
    /* Build control word: EN=1, TRIGGER=0 (SW), STYPE=2 (word), DTYPE=2 (word) */
    uint32_t ctrl = (1u  <<  0)   /* EN      */
                  | (0u  <<  8)   /* TRIGGER = SW */
                  | (2u  << 16)   /* STYPE   = word */
                  | ((uint32_t)sai << 18)  /* SAI */
                  | (2u  << 24)   /* DTYPE   = word */
                  | ((uint32_t)dai << 26); /* DAI */

    MS_DMAC_setControlReg(DMAC_BASE, (int)ctrl);
    MS_DMAC_setSourceAddr(DMAC_BASE, (int)src);
    MS_DMAC_setDestinationAddr(DMAC_BASE, (int)dst);
    MS_DMAC_setCount(DMAC_BASE, count);

    MS_DMAC_setSWTrigger(DMAC_BASE, 1);
    while (MS_DMAC_getSWTrigger(DMAC_BASE))
        ; /* wait for SWTRIG to auto-clear on completion */
}

/* ---- Test implementations ---- */

static void test1_word_memcpy(void)
{
    uart_puts("[Test 1] Word memcpy (4 words, SAI=+4 DAI=+4)\r\n");

    for (int i = 0; i < 4; i++) t1_dst[i] = 0;

    /* COUNT=3 → 4 transfers */
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

    /* SAI=0: same source word read 4 times; COUNT=3 → 4 writes */
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

    /* COUNT=7 → 8 transfers */
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

/* ---- Test 4: PIRQ-triggered DMA — I2S FIFO flush (repeated) ---- */

/*
 * Number of 8-word FIFO batches to capture.  16 batches = 128 samples,
 * which covers the first ~4.9 cycles of the 26-word debug_audio.hex pattern
 * (234 total samples, 9 repetitions) and is safely within SRAM budget.
 */
#define T4_N_BATCHES 16

static volatile uint32_t t4_capture[8];  /* reused for every batch */

/*
 * ISR for i2s_irq (IRQ 1, enabled via meiea[1]).
 * With EXTENSION_XH3IRQ=1, only i2s_irq reaches this handler — uart_irq and
 * dmac_irq are masked out by meiea.  i2s_irq fires when the FIFO fills and
 * the DMA burst starts simultaneously (HW PIRQ trigger).  The ISR may be
 * entered a few cycles before the last AHB write lands, so it waits on the
 * final capture word before signalling the main loop.
 */
static volatile int t4_batch_done;

static void __attribute__((interrupt("machine"))) dma_batch_isr(void)
{
    while (t4_capture[7] == 0x5A5A5A5Au)
        ;
    MS_DMAC_enable(DMAC_BASE, 0); /* prevent re-trigger while printing */
    t4_batch_done = 1;
}

/*
 * CPU busy-work run concurrently with each DMA burst.
 * A 32-bit LCG step is cheap but not eliminable by the compiler (output is
 * printed), so it faithfully represents CPU cycles available during DMA.
 * Both the iteration count and the accumulated state are printed to prove
 * the work ran; a zero count means the DMA completed before the CPU reached
 * the work loop (extremely fast transfer or slow re-arm path).
 */
static uint32_t cpu_work_step(uint32_t state)
{
    /* Knuth multiplicative LCG — single multiply+add, no branches */
    return state * 1664525u + 1013904223u;
}

static void test4_pirq_i2s_dma(void)
{
    uart_puts("[Test 4] PIRQ-triggered DMA: I2S FIFO \xe2\x86\x92 SRAM (CPU works in parallel)\r\n");
    uart_printf("  Capturing 0x%x batches x 8 words = 0x%x samples\r\n",
                (uint32_t)T4_N_BATCHES, (uint32_t)(T4_N_BATCHES * 8));
    uart_puts("  (Compare hex values against sim/debug_audio.hex)\r\n\r\n");
    uart_puts("  While each DMA burst runs the CPU advances an LCG checksum.\r\n");
    uart_puts("  'cpu_iters' = work loop iterations completed before i2s_irq.\r\n\r\n");

    /*
     * CTRL word for PIRQ mode:
     *   EN=1         bit  0
     *   TRIGGER=0001 bits 11:8  → armed on PIRQ[0] (i2s_irq)
     *   STYPE=2      bits 17:16 → word reads from FIFO
     *   SAI=0        bits 20:18 → fixed source (FIFO reg, no increment)
     *   DTYPE=2      bits 25:24 → word writes to SRAM
     *   DAI=4        bits 28:26 → +4 B per step (word increment)
     */
    uint32_t ctrl_pirq = (1u  <<  0)   /* EN              */
                       | (1u  <<  8)   /* TRIGGER = PIRQ[0] */
                       | (2u  << 16)   /* STYPE   = word  */
                       | (0u  << 18)   /* SAI     = 0     */
                       | (2u  << 24)   /* DTYPE   = word  */
                       | (4u  << 26);  /* DAI     = +4    */

    /* Configure I2S: start generating frames and asserting i2s_irq on FIFO full */
    volatile i2s_hw_t *i2s = (volatile i2s_hw_t *)I2S_BASE;
    i2s->conf = (4u << I2S_CONF_DIV_LSB) | (1u << I2S_CONF_IRQ_EN_LSB);

    /* Route i2s_irq exclusively to our ISR via MEIEA; MIE enabled per-batch below */
    uintptr_t saved_mtvec = csr_get_mtvec();
    csr_set_mtvec(dma_batch_isr);
    csr_meiea_i2s_en();
    csr_meie_en();
    /* mstatus.MIE stays 0 here — enabled only after DMA is armed each iteration */

    int      pass            = 1;
    uint32_t total_cpu_iters = 0;   /* LCG iterations across all batches */
    uint32_t cpu_state       = 0xDEADBEEFu; /* LCG accumulator — printed to prevent DCE */

    for (int batch = 0; batch < T4_N_BATCHES; batch++) {

        /* mstatus.MIE=0 during re-arm: prevents ISR firing on a stale i2s_irq
         * while sentinels are written but the DMA is not yet armed. */
        for (int i = 0; i < 8; i++) t4_capture[i] = 0x5A5A5A5Au;
        t4_batch_done = 0;

        MS_DMAC_setControlReg(DMAC_BASE, (int)(ctrl_pirq & ~1u)); /* EN=0 */
        MS_DMAC_setSourceAddr(DMAC_BASE, (int)I2S_FIFO_REG);
        MS_DMAC_setDestinationAddr(DMAC_BASE, (int)t4_capture);
        MS_DMAC_setCount(DMAC_BASE, 7);                           /* COUNT=7 → 8 transfers */
        MS_DMAC_setControlReg(DMAC_BASE, (int)ctrl_pirq);         /* EN=1 — DMA armed */

        /* Enable MIE now that DMA is armed.
         * The DMA fires via hardware PIRQ independently of MIE, so it may
         * already have completed by the time we reach the work loop — but
         * t4_batch_done is set only by the ISR, which cannot run until MIE=1.
         * Using do-while on t4_batch_done guarantees the CPU executes at least
         * one LCG step before the ISR gets a chance to set the flag. */
        csr_mie_en();

        /*
         * CPU work loop — runs between MIE enable and the ISR firing.
         * On real hardware (8 kHz I2S, 36 MHz SoC): the FIFO takes ~1 ms to
         * fill, giving ~7 200 LCG iterations before i2s_irq asserts.
         * In simulation the ISR fires almost immediately after mie_en, so
         * expect a small but nonzero count (interrupt latency + ISR spin time).
         */
        uint32_t batch_iters = 0;
        do {
            cpu_state = cpu_work_step(cpu_state);
            batch_iters++;
        } while (!t4_batch_done);

        /* Ensure EN=0 whether completion came via ISR or the fast-path above */
        MS_DMAC_enable(DMAC_BASE, 0);
        csr_mie_dis(); /* keep MIE=0 during re-arm of next iteration */

        total_cpu_iters += batch_iters;

        if (t4_capture[7] == 0x5A5A5A5Au) {
            uart_printf("  Batch 0x%x: INCOMPLETE (DMA did not fill capture buffer)\r\n",
                        (uint32_t)batch);
            pass = 0;
            break;
        }

        /* Print batch data with CPU parallelism evidence. */
        uart_printf("  Batch 0x%x (samples 0x%x-0x%x)  cpu_iters=0x%x:\r\n",
                    (uint32_t)batch,
                    (uint32_t)(batch * 8),
                    (uint32_t)(batch * 8 + 7),
                    batch_iters);
        for (int i = 0; i < 8; i++) {
            uart_printf("    [0x%x] 0x%x\r\n",
                        (uint32_t)(batch * 8 + i), t4_capture[i]);
            if (t4_capture[i] == 0x5A5A5A5Au) pass = 0; /* sentinel unchanged = fail */
        }
    }

    uart_printf("  Total CPU work: 0x%x LCG iters, final state=0x%x\r\n",
                total_cpu_iters, cpu_state);

    csr_meie_dis();
    csr_meiea_i2s_dis();
    csr_set_mtvec((void (*)(void))saved_mtvec);

    /* Stop I2S */
    i2s->conf = 0;

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
