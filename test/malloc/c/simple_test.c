#include <stdint.h>
#include <stddef.h>
#include "../../common/uart_utils.h"

/* Forward declarations of malloc family functions */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

static void fill_pattern(uint8_t *p, size_t n, uint8_t seed) {
    size_t i;
    for (i = 0; i < n; ++i) {
        p[i] = (uint8_t)(seed + i);
    }
}

static int check_pattern(const uint8_t *p, size_t n, uint8_t seed) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (p[i] != (uint8_t)(seed + i)) {
            return 0;
        }
    }
    return 1;
}

static int check_zero(const uint8_t *p, size_t n) {
    size_t i;
    for (i = 0; i < n; ++i) {
        if (p[i] != 0) {
            return 0;
        }
    }
    return 1;
}

int main(void) {
    uint8_t *a;
    uint8_t *b;
    uint8_t *c;
    uint8_t *z;
    uint8_t *r;

    uart_init();
    printf("malloc test begin\r\n");

    a = (uint8_t *)malloc(32);
    b = (uint8_t *)malloc(48);
    c = (uint8_t *)malloc(64);
    if (a == NULL || b == NULL || c == NULL) {
        printf("FAIL: alloc\r\n");
        return 1;
    }

    fill_pattern(a, 32, 0x10);
    fill_pattern(b, 48, 0x40);
    fill_pattern(c, 64, 0x80);
    if (!check_pattern(a, 32, 0x10) ||
        !check_pattern(b, 48, 0x40) ||
        !check_pattern(c, 64, 0x80)) {
        printf("FAIL: pattern\r\n");
        return 2;
    }

    z = (uint8_t *)calloc(24, 1);
    if (z == NULL || !check_zero(z, 24)) {
        printf("FAIL: calloc\r\n");
        return 3;
    }

    fill_pattern(a, 32, 0x20);
    r = (uint8_t *)realloc(a, 96);
    if (r == NULL || !check_pattern(r, 32, 0x20)) {
        printf("FAIL: realloc\r\n");
        return 4;
    }
    a = r;

    free(b);
    b = (uint8_t *)malloc(40);
    if (b == NULL) {
        printf("FAIL: reuse\r\n");
        return 5;
    }
    fill_pattern(b, 40, 0x55);
    if (!check_pattern(b, 40, 0x55)) {
        printf("FAIL: reuse pattern\r\n");
        return 6;
    }

    free(a);
    free(b);
    free(c);
    free(z);

    printf("PASS: malloc\r\n");
    return 0;
}
