#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdarg.h>

// --- UART REGISTER DEFINITIONS ---
#define UART_BASE  0x40004000
#define UART_CSR   (*(volatile uint32_t *)(UART_BASE + 0x00))
#define UART_DIV   (*(volatile uint32_t *)(UART_BASE + 0x04))
// Assumes FSTAT is at offset 0x08 based on previous context
#define UART_FSTAT (*(volatile uint32_t *)(UART_BASE + 0x08))
#define UART_TX    (*(volatile uint32_t *)(UART_BASE + 0x0C))

#define UART_CSR_EN       (1 << 0)
#define UART_FSTAT_TXFULL (1 << 8) // Validated against "fstat_txfull_i" in Verilog

// --- CONFIGURATION ---
#ifndef CLK_MHZ
    #error "CLK_MHZ is not defined. Check your Makefile variables!"
#endif
#ifndef UART_BAUD_RATE
    #error "CLK_MHZ is not defined. Check your Makefile variables!"
#endif

// --- BAUD CALCULATION ---
// Convert MHz to Hz
#define SYS_CLK_HZ  ((uint32_t)(CLK_MHZ * 1000000UL))

// Calculate Fractional Divisor - Based on uart_mini
// Hardware requires 8x Oversampling and uses 4 fractional bits (x16 scale).
// Formula: (CLK / (Baud * 8)) * 16  ==>  (CLK / Baud) * 2
// We apply rounding logic: ((2*CLK) + (Baud/2)) / Baud
#define UART_DIV_VAL  (((2 * SYS_CLK_HZ) + (UART_BAUD_RATE / 2)) / UART_BAUD_RATE)

void uart_init(void);
void uart_putc(char c);
// Print a null-terminated string
void uart_puts(const char *s);

void uart_puthex(uint32_t val);
// Tiny formatted print (supports %s, %c, %x, %%)
void uart_printf(const char *format, ...);

#endif
