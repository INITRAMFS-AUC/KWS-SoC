// Bridge bug regression test for the 2-port CPU + APB-bridge load→store-data
// race documented in docs/2port_dport_bridge_bug.md.
//
// Behaviour:
//   - clean_write(0x55) x4   — sync header (uses an inline asm with NOPs
//                              between the LBU of c and the SW to UART_TX,
//                              so it never triggers the bug)
//   - buggy_write(b) for b = 0x00..0xFF  — emits the exact LBU c → tight
//                              SW UART_TX = c pattern that triggered the
//                              corruption-to-0xD0 on FPGA before the fix
//   - clean_write(0xAA) x4   — end-of-round marker
//   - ebreak                  — halt so a host can dump state via GDB
//
// On a working build the receiver should see:
//   55 55 55 55  00 01 02 ... ff  aa aa aa aa
//
// On a broken build (race fires) every byte in the middle is 0xD0 and the
// sync/marker bytes still come through cleanly via clean_write.

#include <stdint.h>

#define UART_BASE  0x40004000
#define UART_CSR   (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV   (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_FSTAT (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_TX    (*(volatile uint32_t *)(UART_BASE + 0x0C))

#define UART_CSR_EN       (1 << 0)
#define UART_FSTAT_TXFULL (1 << 8)

#ifndef CLK_MHZ
    #error "CLK_MHZ is not defined. Check your Makefile variables!"
#endif
#ifndef UART_BAUD_RATE
    #error "UART_BAUD_RATE is not defined. Check your Makefile variables!"
#endif

#define SYS_CLK_HZ    ((uint32_t)(CLK_MHZ * 1000000UL))
#define UART_DIV_VAL  (((2 * SYS_CLK_HZ) + (UART_BAUD_RATE / 2)) / UART_BAUD_RATE)

static void uart_init(void) {
    UART_DIV = UART_DIV_VAL;
    UART_CSR |= UART_CSR_EN;
}

// Triggers the race: heavy d-port loop -> tight LBU c -> SW UART_TX = c
__attribute__((noinline))
static void buggy_write(char c) {
    for (volatile uint32_t i = 0; i < 200000; i++);
    UART_TX = c;
}

// Avoids the race: forces NOPs between the LBU of c and the SW to UART_TX.
// Useful as a control byte the receiver can rely on regardless of the bug.
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

    // Sync header: 4 known-good 0x55 bytes via clean_write
    for (int s = 0; s < 4; s++) clean_write((char)0x55);

    // Test body: 256 bytes 0x00..0xFF via the buggy pattern
    for (uint32_t b = 0; b < 256; b++) {
        buggy_write((char)b);
    }

    // End-of-round marker: 4 known-good 0xAA bytes via clean_write
    for (int s = 0; s < 4; s++) clean_write((char)0xAA);

    // Halt so GDB can read SRAM state
    asm volatile ("ebreak");
    while (1);
    return 0;
}
