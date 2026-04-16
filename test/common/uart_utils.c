#include <stdint.h>
#include "uart_utils.h"

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

#define SYS_CLK_HZ  ((uint32_t)(CLK_MHZ * 1000000UL))
#define UART_DIV_VAL  (((2 * SYS_CLK_HZ) + (UART_BAUD_RATE / 2)) / UART_BAUD_RATE)

void uart_init() {
    UART_DIV = UART_DIV_VAL;
    UART_CSR |= UART_CSR_EN;
}

void uart_putc(char c) {
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    UART_TX = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

int printf(const char *s) {
    uart_puts(s);
    return 0;
}
