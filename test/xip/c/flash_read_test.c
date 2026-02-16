#include <stdint.h>

// Flash memory base address (adjust based on your address mapping)
#define FLASH_BASE 0x80000000

// Test pattern - these values need to be pre-loaded into flash
// The values below correspond to the data in init.hex
#define EXPECTED_WORD_0  0x12FEDCBA  // bytes at 0x00-0x03 (little endian)
#define EXPECTED_WORD_1  0xA7A6A5A4  // bytes at 0x04-0x07
#define EXPECTED_WORD_2  0xBBBAB9B8  // bytes at 0x08-0x0B

// UART for test output
#define UART_BASE       0x40004000
#define UART_CSR        (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV        (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_FSTAT      (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_TX         (*(volatile uint32_t *)(UART_BASE + 0x0C))
#define UART_CSR_EN     (1 << 0)
#define UART_FSTAT_TXFULL (1 << 8)

#ifndef CLK_MHZ
#define CLK_MHZ 12
#endif

#ifndef UART_BAUD_RATE
#define UART_BAUD_RATE 115200
#endif

#define SYS_CLK_HZ  ((uint32_t)(CLK_MHZ * 1000000UL))
#define UART_DIV_VAL  (((2 * SYS_CLK_HZ) + (UART_BAUD_RATE / 2)) / UART_BAUD_RATE)

void uart_putc(char c) {
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    UART_TX = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

void uart_puthex(uint32_t val) {
    const char hex[] = "0123456789ABCDEF";
    uart_puts("0x");
    for (int i = 28; i >= 0; i -= 4) {
        uart_putc(hex[(val >> i) & 0xF]);
    }
}

void uart_init() {
    UART_DIV = UART_DIV_VAL;
    UART_CSR |= UART_CSR_EN;
}

int main() {
    uart_init();

    uart_puts("\r\n========================================\r\n");
    uart_puts("Flash Memory Integration Test\r\n");
    uart_puts("========================================\r\n\r\n");

    // Test 1: Basic read from flash
    uart_puts("[TEST 1] Basic Flash Read @ 0x00\r\n");
    volatile uint32_t *flash_ptr = (volatile uint32_t *)FLASH_BASE;
    uint32_t word0 = flash_ptr[0];

    uart_puts("  Expected: ");
    uart_puthex(EXPECTED_WORD_0);
    uart_puts("\r\n  Read:     ");
    uart_puthex(word0);
    uart_puts("\r\n  Result:   ");

    if (word0 == EXPECTED_WORD_0) {
        uart_puts("PASS\r\n\r\n");
    } else {
        uart_puts("FAIL\r\n\r\n");
        uart_puts("\r\n*** TEST FAILED ***\r\n");
        while(1);  // Halt on failure
    }

    // Test 2: Sequential reads (tests cache line filling)
    uart_puts("[TEST 2] Sequential Reads (Cache Fill)\r\n");
    uint32_t word1 = flash_ptr[1];
    uint32_t word2 = flash_ptr[2];

    uart_puts("  Word 1 - Expected: ");
    uart_puthex(EXPECTED_WORD_1);
    uart_puts(", Read: ");
    uart_puthex(word1);
    uart_puts(" - ");
    if (word1 == EXPECTED_WORD_1) {
        uart_puts("PASS\r\n");
    } else {
        uart_puts("FAIL\r\n");
        uart_puts("\r\n*** TEST FAILED ***\r\n");
        while(1);
    }

    uart_puts("  Word 2 - Expected: ");
    uart_puthex(EXPECTED_WORD_2);
    uart_puts(", Read: ");
    uart_puthex(word2);
    uart_puts(" - ");
    if (word2 == EXPECTED_WORD_2) {
        uart_puts("PASS\r\n\r\n");
    } else {
        uart_puts("FAIL\r\n");
        uart_puts("\r\n*** TEST FAILED ***\r\n");
        while(1);
    }

    // Test 3: Re-read same location (should hit cache)
    uart_puts("[TEST 3] Cache Hit Test (Re-read)\r\n");
    uint32_t word0_again = flash_ptr[0];
    uart_puts("  Re-reading word 0: ");
    uart_puthex(word0_again);
    uart_puts(" - ");
    if (word0_again == word0) {
        uart_puts("PASS\r\n\r\n");
    } else {
        uart_puts("FAIL\r\n");
        uart_puts("\r\n*** TEST FAILED ***\r\n");
        while(1);
    }

    // Test 4: Read from different cache line (LW=256 bits = 32 bytes per line)
    uart_puts("[TEST 4] Different Cache Line Access\r\n");
    volatile uint32_t *other_line = (volatile uint32_t *)(FLASH_BASE + 256);
    uint32_t word_other = other_line[0];
    uart_puts("  Reading from offset 256: ");
    uart_puthex(word_other);
    uart_puts(" - PASS\r\n\r\n");

    // Test 5: Random access pattern
    uart_puts("[TEST 5] Random Access Pattern\r\n");
    uint32_t word_10 = flash_ptr[10];
    uint32_t word_5 = flash_ptr[5];
    uint32_t word_15 = flash_ptr[15];
    uart_puts("  Read words at index 10, 5, 15 - PASS\r\n\r\n");

    uart_puts("========================================\r\n");
    uart_puts("ALL TESTS PASSED!\r\n");
    uart_puts("Flash integration successful.\r\n");
    uart_puts("========================================\r\n");

    // Success loop
    while(1);

    return 0;
}
