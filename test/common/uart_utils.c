#include <stdarg.h>
#include <stddef.h>
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

static int uart_putnch(char ch, int count) {
    int written = 0;
    while (written < count) {
        uart_putc(ch);
        written++;
    }
    return written;
}

static size_t local_strlen(const char *s) {
    size_t n = 0;
    while (s[n] != '\0') {
        n++;
    }
    return n;
}

static int local_is_digit(char c) {
    return c >= '0' && c <= '9';
}

static int local_parse_uint(const char **fmt) {
    int v = 0;
    while (local_is_digit(**fmt)) {
        v = (v << 3) + (v << 1) + (**fmt - '0');
        (*fmt)++;
    }
    return v;
}

static int u32_to_hex_str(uint32_t value, int uppercase, char *out) {
    static const char digits_lo[] = "0123456789abcdef";
    static const char digits_hi[] = "0123456789ABCDEF";
    const char *digits = uppercase ? digits_hi : digits_lo;
    int started = 0;
    int out_len = 0;
    int i;

    for (i = 28; i >= 0; i -= 4) {
        uint32_t nibble = (value >> i) & 0xFU;
        if (nibble != 0U || started) {
            out[out_len++] = digits[nibble];
            started = 1;
        }
    }

    if (!started) {
        out[out_len++] = '0';
    }

    return out_len;
}

static int u32_to_dec_str(uint32_t value, char *out) {
    static const uint32_t powers10[] = {
        1000000000U, 100000000U, 10000000U, 1000000U, 100000U,
        10000U, 1000U, 100U, 10U, 1U
    };
    int started = 0;
    int out_len = 0;
    int i;

    for (i = 0; i < (int)(sizeof(powers10) / sizeof(powers10[0])); i++) {
        uint32_t p = powers10[i];
        int digit = 0;

        while (value >= p) {
            value -= p;
            digit++;
        }

        if (digit != 0 || started || p == 1U) {
            out[out_len++] = (char)('0' + digit);
            started = 1;
        }
    }

    return out_len;
}

static int format_core(const char *num, int num_len,
                       int width, int precision, int left_align, int zero_pad,
                       const char *prefix, int prefix_len) {
    int zeroes;
    int body_len;
    int spaces;
    int written = 0;
    int i;

    zeroes = 0;
    if (precision > num_len) {
        zeroes = precision - num_len;
    } else if (precision < 0 && zero_pad && !left_align && width > (prefix_len + num_len)) {
        zeroes = width - (prefix_len + num_len);
    }

    body_len = prefix_len + zeroes + num_len;
    spaces = width > body_len ? (width - body_len) : 0;

    if (!left_align) {
        written += uart_putnch(' ', spaces);
    }

    for (i = 0; i < prefix_len; i++) {
        uart_putc(prefix[i]);
        written++;
    }

    written += uart_putnch('0', zeroes);

    for (i = 0; i < num_len; i++) {
        uart_putc(num[i]);
        written++;
    }

    if (left_align) {
        written += uart_putnch(' ', spaces);
    }

    return written;
}

static int format_unsigned_u32(uint32_t value, unsigned base, int uppercase,
                               int width, int precision, int left_align, int zero_pad,
                               const char *prefix, int prefix_len) {
    char num[32];
    int num_len = 0;

    if (!(precision == 0 && value == 0U)) {
        if (base == 16U) {
            num_len = u32_to_hex_str(value, uppercase, num);
        } else {
            num_len = u32_to_dec_str(value, num);
        }
    }

    return format_core(num, num_len, width, precision, left_align, zero_pad, prefix, prefix_len);
}

int puts(const char *s) {
    uart_puts(s);
    uart_putc('\n');
    return 0;
}

int putchar(int c) {
    uart_putc((char)c);
    return (unsigned char)c;
}

int printf(const char *fmt, ...) {
    va_list ap;
    int written = 0;

    va_start(ap, fmt);

    while (*fmt != '\0') {
        int left_align = 0;
        int zero_pad = 0;
        int width = 0;
        int precision = -1;
        int length_l = 0;
        char spec;

        if (*fmt != '%') {
            uart_putc(*fmt++);
            written++;
            continue;
        }

        fmt++;
        if (*fmt == '%') {
            uart_putc('%');
            fmt++;
            written++;
            continue;
        }

        while (*fmt == '-' || *fmt == '0') {
            if (*fmt == '-') {
                left_align = 1;
            } else {
                zero_pad = 1;
            }
            fmt++;
        }

        if (local_is_digit(*fmt)) {
            width = local_parse_uint(&fmt);
        }

        if (*fmt == '.') {
            fmt++;
            precision = local_is_digit(*fmt) ? local_parse_uint(&fmt) : 0;
            zero_pad = 0;
        }

        while (*fmt == 'l') {
            length_l++;
            fmt++;
        }

        spec = *fmt;
        if (spec == '\0') {
            break;
        }
        fmt++;

        if (spec == 'd' || spec == 'i') {
            int32_t sval;
            uint32_t uval;
            int neg = 0;
            const char sign_prefix[1] = {'-'};

            if (length_l >= 1) {
                sval = va_arg(ap, long);
            } else {
                sval = va_arg(ap, int);
            }

            if (sval < 0) {
                neg = 1;
                uval = (uint32_t)(-(sval + 1)) + 1U;
            } else {
                uval = (uint32_t)sval;
            }

            written += format_unsigned_u32(
                uval,
                10,
                0,
                width,
                precision,
                left_align,
                zero_pad,
                neg ? sign_prefix : "",
                neg ? 1 : 0
            );
            continue;
        }

        if (spec == 'u' || spec == 'x' || spec == 'X') {
            uint32_t uval;
            unsigned base = (spec == 'u') ? 10U : 16U;
            int upper = (spec == 'X');

            if (length_l >= 1) {
                uval = va_arg(ap, unsigned long);
            } else {
                uval = va_arg(ap, unsigned int);
            }

            written += format_unsigned_u32(uval, base, upper, width, precision, left_align, zero_pad, "", 0);
            continue;
        }

        if (spec == 'p') {
            uint32_t p = (uint32_t)(uintptr_t)va_arg(ap, void *);
            written += format_unsigned_u32(p, 16, 0, width, precision, left_align, zero_pad, "0x", 2);
            continue;
        }

        if (spec == 'c') {
            char c = (char)va_arg(ap, int);
            int spaces = width > 1 ? width - 1 : 0;

            if (!left_align) {
                written += uart_putnch(' ', spaces);
            }
            uart_putc(c);
            written++;
            if (left_align) {
                written += uart_putnch(' ', spaces);
            }
            continue;
        }

        if (spec == 's') {
            const char *s = va_arg(ap, const char *);
            int slen;
            int out_len;
            int spaces;
            int i;

            if (s == NULL) {
                s = "(null)";
            }

            slen = (int)local_strlen(s);
            out_len = (precision >= 0 && precision < slen) ? precision : slen;
            spaces = width > out_len ? width - out_len : 0;

            if (!left_align) {
                written += uart_putnch(' ', spaces);
            }
            for (i = 0; i < out_len; i++) {
                uart_putc(s[i]);
                written++;
            }
            if (left_align) {
                written += uart_putnch(' ', spaces);
            }
            continue;
        }

        uart_putc('%');
        uart_putc(spec);
        written += 2;
    }

    va_end(ap);
    return written;
}
