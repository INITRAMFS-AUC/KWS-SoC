#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    uint8_t v = (uint8_t)value;

    while (n--) {
        *d++ = v;
    }

    return dest;
}