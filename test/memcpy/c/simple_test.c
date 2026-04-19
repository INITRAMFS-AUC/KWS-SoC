#include <stddef.h>
#include <stdint.h>
#include "../../common/uart_utils.h"

extern void *memcpy(void *dest, const void *src, size_t n);

static int expect_eq(const uint8_t *a, const uint8_t *b, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (a[i] != b[i]) {
            return 0;
        }
    }
    return 1;
}

static int expect_val(const uint8_t *a, size_t n, uint8_t v) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (a[i] != v) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    uint8_t src_a[64];
    uint8_t dst_a[64];
    uint8_t src_b[31];
    uint8_t dst_b[31];
    uint8_t *ret;
    void *(*memcpy_fn)(void *, const void *, size_t);
    size_t i;

    uart_init();
    printf("memcpy test begin\r\n");

    memcpy_fn = memcpy;

    for (i = 0; i < sizeof(src_a); ++i) {
        src_a[i] = (uint8_t)(0xA0u + (uint8_t)i);
        dst_a[i] = 0x5Au;
    }

    ret = (uint8_t *)memcpy_fn(dst_a, src_a, sizeof(src_a));
    if (ret != dst_a) {
        printf("FAIL: return value\r\n");
        return 1;
    }
    if (!expect_eq(dst_a, src_a, sizeof(src_a))) {
        printf("FAIL: aligned copy\r\n");
        return 2;
    }

    for (i = 0; i < sizeof(src_b); ++i) {
        src_b[i] = (uint8_t)(0x11u + (uint8_t)(3u * i));
        dst_b[i] = 0u;
    }

    ret = (uint8_t *)memcpy_fn(dst_b + 1, src_b + 1, sizeof(src_b) - 1);
    if (ret != (dst_b + 1)) {
        printf("FAIL: unaligned return\r\n");
        return 3;
    }
    if (!expect_eq(dst_b + 1, src_b + 1, sizeof(src_b) - 1)) {
        printf("FAIL: unaligned copy\r\n");
        return 4;
    }

    for (i = 0; i < sizeof(dst_b); ++i) {
        dst_b[i] = 0x77u;
    }
    ret = (uint8_t *)memcpy_fn(dst_b, src_b, 0);
    if (ret != dst_b) {
        printf("FAIL: zero-len return\r\n");
        return 5;
    }
    if (!expect_val(dst_b, sizeof(dst_b), 0x77u)) {
        printf("FAIL: zero-len changed buffer\r\n");
        return 6;
    }

    for (i = 0; i < sizeof(src_a); ++i) {
        src_a[i] = (uint8_t)i;
    }
    ret = (uint8_t *)memcpy_fn(src_a, src_a, sizeof(src_a));
    if (ret != src_a || !expect_eq(src_a, src_a, sizeof(src_a))) {
        printf("FAIL: self copy\r\n");
        return 7;
    }

    printf("PASS: memcpy\r\n");
    return 0;
}