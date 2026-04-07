#include "uart.h"

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
