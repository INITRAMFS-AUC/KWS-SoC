#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n) {
    uint8_t *d = (uint8_t *)dest;
    const uint8_t *s = (const uint8_t *)src;

    if (n == 0 || d == s) {
        return dest;
    }

    while (n--) {
        *d++ = *s++;
    }

    return dest;
}