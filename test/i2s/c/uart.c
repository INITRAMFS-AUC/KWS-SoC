#include "uart.h"
#include "uart_regs.h"

#define UART_REG(off) (*(volatile uint32_t *)(UART_BASE + (off)))

void uart_init() {
    UART_REG(UART_DIV_OFFS)  = UART_DIV_VAL;
    UART_REG(UART_CSR_OFFS) |= UART_CSR_EN_MASK;
}

void uart_putc(char c) {
    while (UART_REG(UART_FSTAT_OFFS) & UART_FSTAT_TXFULL_MASK);
    UART_REG(UART_TX_OFFS) = c;
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
            uart_putc(nibble < 10 ? '0' + nibble : 'a' + (nibble - 10));
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
                case 's': uart_puts(va_arg(args, const char *)); break;
                case 'c': uart_putc((char)va_arg(args, int));    break;
                case 'x': uart_puthex(va_arg(args, uint32_t));  break;
                case '%': uart_putc('%');                        break;
                default:  uart_putc(*format);                   break;
            }
        } else {
            uart_putc(*format);
        }
        format++;
    }

    va_end(args);
}
