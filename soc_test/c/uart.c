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

void uart_init() {
    // Set the fractional divisor
    UART_DIV = UART_DIV_VAL;

    // Enable UART (Loopback, Interrupts etc. handled here if needed)
    UART_CSR |= UART_CSR_EN;
}

void uart_putc(char c) {
    // Wait for TX FIFO to be not full
    while (UART_FSTAT & UART_FSTAT_TXFULL);
    UART_TX = c;
}

void uart_puts(const char *s) {
    while (*s) uart_putc(*s++);
}

// Helper to print a 32-bit integer in hexadecimal
void uart_puthex(uint32_t val) {
    if (val == 0) {
        uart_putc('0');
        return;
    }
    
    int started = 0;
    // Iterate through nibbles from highest to lowest
    for (int i = 28; i >= 0; i -= 4) {
        uint8_t nibble = (val >> i) & 0xF;
        
        // Skip leading zeros
        if (nibble != 0 || started) {
            started = 1;
            if (nibble < 10) {
                uart_putc('0' + nibble);
            } else {
                uart_putc('a' + (nibble - 10)); // Use 'A' instead of 'a' for uppercase hex
            }
        }
    }
}

// Tiny formatted print implementation
void uart_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    while (*format) {
        if (*format == '%') {
            format++;
            switch (*format) {
                case 's':
                    uart_puts(va_arg(args, const char *));
                    break;
                case 'c':
                    uart_putc((char)va_arg(args, int)); 
                    break;
                case 'x':
                    uart_puthex(va_arg(args, uint32_t));
                    break;
                case '%':
                    uart_putc('%');
                    break;
                default:
                    // If format specifier isn't recognized, print the raw char
                    uart_putc(*format); 
                    break;
            }
        } else {
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}
