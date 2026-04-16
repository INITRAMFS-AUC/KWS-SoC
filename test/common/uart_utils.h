#ifndef UART_UTILS_H
#define UART_UTILS_H

void uart_init(void);
void uart_putc(char c);
void uart_puts(const char *s);
int printf(const char *s);

#endif // UART_UTILS_H
