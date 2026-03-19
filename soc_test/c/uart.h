#ifndef UART_H
#define UART_H

#include <stdint.h>

void uart_init(void);
void uart_putc(char c);
// Print a null-terminated string
void uart_puts(const char *s);

void uart_puthex(uint32_t val);
// Tiny formatted print (supports %s, %c, %x, %%)
void uart_printf(const char *format, ...);

#endif
