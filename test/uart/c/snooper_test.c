// Snooper test + UART self-dump.
//
// Runs the same clean/buggy-write pattern as bridge_bug_test, with the
// bus_snooper at 0x4000_C000 armed, then disables the snooper and streams
// the 16 ring entries over UART as hex so the same firmware can be run in
// Verilator AND on FPGA and the captures compared byte-for-byte.
//
// Output format (after the run):
//   "SNOOP\r\n"
//   16 lines, one per entry, each: 8 hex words concatenated + "\r\n"
//     word offsets within an entry:
//       +0x00 cycle
//       +0x04 addr_ctrl (bridge / dport / aphq / xm_memop / m_bus_stall)
//       +0x08 bridge_hwdata
//       +0x0C dbg_m_wdata
//       +0x10 {mw_rd, xm_rs2}
//       +0x14 dbg_xm_result
//       +0x18 dbg_mw_result
//       +0x1C dport_haddr
//   "END\r\n"
//
// All UART writes use a SAFE pattern (FSTAT-poll then asm-block SW with NOPs
// between any LBU and the SW to UART_TX) so the dump itself cannot trigger
// the d-port → bridge double-aphase symptom we're investigating.

#include <stdint.h>

#define UART_BASE   0x40004000
#define UART_CSR    (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV    (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_FSTAT  (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_CSR_EN (1 << 0)
#define UART_FSTAT_TXFULL (1 << 8)

#define SNOOP_BASE  0x4000C000
#define SNOOP_CTRL  (*(volatile uint32_t *)(SNOOP_BASE + 0x200))
#define SNOOP_RESET (*(volatile uint32_t *)(SNOOP_BASE + 0x20C))

#ifndef CLK_MHZ
    #error "CLK_MHZ is not defined."
#endif
#ifndef UART_BAUD_RATE
    #error "UART_BAUD_RATE is not defined."
#endif

#define SYS_CLK_HZ    ((uint32_t)(CLK_MHZ * 1000000UL))
#define UART_DIV_VAL  (((2 * SYS_CLK_HZ) + (UART_BAUD_RATE / 2)) / UART_BAUD_RATE)

static void uart_init(void) {
    UART_DIV  = UART_DIV_VAL;
    UART_CSR |= UART_CSR_EN;
}

// ---- The trigger pattern (same as bridge_bug_test). -----------------------

// Tight LBU→SW to UART_TX. Compiler emits exactly the failing instruction
// sequence (lbu c, [stack]; sw c, 0x4000_400c) with no register ops between.
__attribute__((noinline))
static void buggy_write(char c) {
    for (volatile uint32_t i = 0; i < 200000; i++);
    *(volatile uint32_t *)(UART_BASE + 0x0C) = (uint32_t)(unsigned char)c;
}

__attribute__((noinline))
static void clean_write(char c) {
    for (volatile uint32_t i = 0; i < 200000; i++);
    asm volatile (
        "nop\nnop\nnop\nnop\n"
        "lui  t1, 0x40004\n"
        "addi t1, t1, 12\n"
        "sw   %0, 0(t1)\n"
        :
        : "r" ((uint32_t)(unsigned char)c)
        : "t1", "memory"
    );
}

// ---- The safe TX path used by the dump. -----------------------------------
// FSTAT-poll (load + branch only — no SW), then an asm block that places NOPs
// before the LBU/SW pair, the address constant materialised in t1 from
// immediates, and a SW to UART_TX. Compiler can't squeeze a tight LBU→SW
// across this boundary.

__attribute__((noinline))
static void uart_putc_safe(unsigned char c) {
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    asm volatile (
        "nop\nnop\nnop\nnop\n"
        "lui  t1, 0x40004\n"
        "addi t1, t1, 12\n"
        "sw   %0, 0(t1)\n"
        :
        : "r" ((uint32_t)c)
        : "t1", "memory"
    );
}

static void put_str(const char *s) {
    while (*s) uart_putc_safe((unsigned char)*s++);
}

static void put_hex32(uint32_t v) {
    static const char hex[] = "0123456789abcdef";
    int i;
    for (i = 28; i >= 0; i -= 4)
        uart_putc_safe((unsigned char)hex[(v >> i) & 0xf]);
}

// ---------------------------------------------------------------------------

int main(void) {
    uart_init();

    // Banner before the test so the receiver can sync (also confirms UART
    // is alive on this build).
    put_str("BEGIN\r\n");

    // Arm snooper *after* uart_init / banner so the DIV / CSR / banner UART
    // writes don't burn ring slots.
    SNOOP_RESET = 1;
    SNOOP_CTRL  = 1;

    // The trigger pattern. 8 transactions: alternating clean/buggy.
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);

    // Freeze the ring so the dump reads stable values.
    SNOOP_CTRL = 0;

    // Dump: 16 entries, 8 words per entry, hex packed, no separators within
    // an entry — keeps the per-entry line a constant 8*8 = 64 hex chars +
    // CRLF, easy to parse.
    put_str("\r\nSNOOP\r\n");
    for (int e = 0; e < 16; e++) {
        uint32_t base = SNOOP_BASE + (uint32_t)(e * 32);
        for (int w = 0; w < 8; w++) {
            put_hex32(*(volatile uint32_t *)(base + (uint32_t)(w * 4)));
        }
        put_str("\r\n");
    }
    put_str("END\r\n");

    asm volatile ("ebreak");
    while (1);
    return 0;
}
