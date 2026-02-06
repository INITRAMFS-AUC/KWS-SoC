#include <stdint.h>

// UART registers
#define UART_BASE 0x40004000
#define UART_CSR  (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV  (*(volatile uint32_t *)(UART_BASE + 0x04))
#define UART_FSTAT (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_TX   (*(volatile uint32_t *)(UART_BASE + 0x0C))

// CSR bits
#define UART_CSR_EN       (1 << 0)
#define UART_CSR_TXIE     (1 << 2)

// FSTAT bits
#define UART_FSTAT_TXFULL (1 << 8)

void uart_init() {
    // TODO: Make this work on both sim and quartus (i.e. sane baud rate for both)
    UART_DIV = 0x271; // Remove this line for it to work on sim
    // Enable UART
    UART_CSR |= UART_CSR_EN;
}

void uart_putc(char c) {
    // Wait for TX FIFO to be not full
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    UART_TX = c;
}

void uart_puts(const char *s) {
    while (*s) {
        uart_putc(*s++);
    }
}

int main() {
    uart_init();
    while (1) {
        uart_puts("Hello World!\n");
    }
    return 0;
}
