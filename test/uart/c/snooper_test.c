// Snooper test: arms the bus_snooper at 0x4000_C000, runs a short
// clean/buggy alternating pattern of UART_TX writes, halts via ebreak so
// a host can read back the 16-entry ring buffer over JTAG SBA.
//
// What to expect on a working build:
//   - bridge_hwdata == m_wdata == the actual char (0x55 / 0x42 / ...).
//   - xm_rs2 == 5'ha4 nibble (or whichever rs2 reg gcc picks for the SW).
//   - mw_rd == 5'ha5 nibble (or whichever rd reg gcc picks for the LBU).
//
// What to expect on the broken build (FPGA, 2-port + tight LBU→SW):
//   - For buggy_write entries, bridge_hwdata low byte == 0xD0 (corrupted),
//     while m_wdata low byte == the intended char.
//   - mw_rd != xm_rs2 (bypass condition false), m_wdata == xm_result
//     (which for a load is the load's address — leaks 0xD0 = (s0+offs)[7:0]).
//
// Read out from GDB after ebreak:
//   x/8wx 0x4000C000     # entry 0 (cycle, addr_ctrl, hwdata, m_wdata, idx, xm_result, mw_result, _)
//   x/8wx 0x4000C020     # entry 1
//   ...
//   x/wx  0x4000C204     # STAT  = {head[3:0]<<8 | count[4:0]}

#include <stdint.h>

#define UART_BASE  0x40004000
#define UART_CSR   (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV   (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_TX    (*(volatile uint32_t *)(UART_BASE + 0x0C))
#define UART_CSR_EN (1 << 0)

#define SNOOP_BASE  0x4000C000
#define SNOOP_CTRL  (*(volatile uint32_t *)(SNOOP_BASE + 0x200))
#define SNOOP_STAT  (*(volatile uint32_t *)(SNOOP_BASE + 0x204))
#define SNOOP_CYCLE (*(volatile uint32_t *)(SNOOP_BASE + 0x208))
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

// Same race trigger as bridge_bug_test.c: heavy d-port loop, then a tight
// LBU of c, then SW UART_TX = c with no register ops in between.
__attribute__((noinline))
static void buggy_write(char c) {
    for (volatile uint32_t i = 0; i < 200000; i++);
    UART_TX = c;
}

// Same control as bridge_bug_test.c: forces NOPs (in asm) between the
// load of c and the SW to UART_TX, hiding the race.
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

int main(void) {
    uart_init();

    // Arm the snooper *after* uart_init so the DIV / CSR writes don't
    // burn ring slots. Clear any prior state, then enable.
    SNOOP_RESET = 1;
    SNOOP_CTRL  = 1;

    // 8 transactions: alternating clean/buggy. Fits comfortably inside the
    // 16-entry ring with room for whatever the compiler emits between calls.
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);
    clean_write(0x55);
    buggy_write(0x42);

    // Stop capturing so the ring freezes; GDB reads it out from here.
    SNOOP_CTRL = 0;

    asm volatile ("ebreak");
    while (1);
    return 0;
}
