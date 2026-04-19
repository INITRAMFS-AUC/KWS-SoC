#include <stdint.h>
#include <stddef.h>
#include "../../common/uart_utils.h"

/* Forward declarations of malloc family functions */
void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);

#define STRESS_SLOTS 32u
#define STRESS_ITERS 10000u

static uint32_t prng_next(uint32_t *state) {
    uint32_t x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static uint32_t bounded_rand(uint32_t *state, uint32_t bound) {
    uint32_t mask;
    uint32_t x;

    if (bound == 0u) {
        return 0u;
    }

    mask = 1u;
    while (mask < (bound - 1u)) {
        mask = (mask << 1) | 1u;
    }

    do {
        x = prng_next(state) & mask;
    } while (x >= bound);

    return x;
}

static uint8_t pattern_seed_from_ptr(const void *p, uint32_t nonce) {
    uintptr_t v = (uintptr_t)p;
    return (uint8_t)((v ^ (v >> 8) ^ nonce) & 0xffu);
}

static int nth_active_slot(const uint8_t *active, size_t n) {
    size_t i;
    for (i = 0; i < STRESS_SLOTS; ++i) {
        if (active[i]) {
            if (n == 0) {
                return (int)i;
            }
            --n;
        }
    }
    return -1;
}

static int first_free_slot(const uint8_t *active) {
    size_t i;
    for (i = 0; i < STRESS_SLOTS; ++i) {
        if (!active[i]) {
            return (int)i;
        }
    }
    return -1;
}

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
    uint8_t *slots[STRESS_SLOTS];
    size_t sizes[STRESS_SLOTS];
    uint8_t seeds[STRESS_SLOTS];
    uint8_t active[STRESS_SLOTS];
    uint32_t rng;
    uint32_t iter;
    size_t active_count;
    size_t i;

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

    for (i = 0; i < STRESS_SLOTS; ++i) {
        slots[i] = NULL;
        sizes[i] = 0;
        seeds[i] = 0;
        active[i] = 0;
    }

    rng = 0x6d2b79f5u;
    active_count = 0;

    for (iter = 0; iter < STRESS_ITERS; ++iter) {
        uint32_t rv = prng_next(&rng);
        int do_alloc = (rv & 1u) != 0u;

        if (active_count == 0) {
            do_alloc = 1;
        } else if (active_count == STRESS_SLOTS) {
            do_alloc = 0;
        }

        if (do_alloc) {
            int slot = first_free_slot(active);
            size_t sz;
            uint8_t *p;
            uint8_t seed;
            uint32_t cls;

            if (slot < 0) {
                printf("FAIL: stress slot\r\n");
                return 7;
            }

            cls = bounded_rand(&rng, 10u);
            if (cls < 6u) {
                sz = (size_t)(bounded_rand(&rng, 120u) + 8u);
            } else if (cls < 9u) {
                sz = (size_t)(bounded_rand(&rng, 896u) + 128u);
            } else {
                sz = (size_t)(bounded_rand(&rng, 3073u) + 1024u);
            }
            p = (uint8_t *)malloc(sz);
            if (p == NULL) {
                continue;
            }

            seed = pattern_seed_from_ptr(p, prng_next(&rng));
            fill_pattern(p, sz, seed);

            slots[slot] = p;
            sizes[slot] = sz;
            seeds[slot] = seed;
            active[slot] = 1;
            ++active_count;
        } else {
            size_t nth = (size_t)bounded_rand(&rng, (uint32_t)active_count);
            int slot = nth_active_slot(active, nth);

            if (slot < 0) {
                printf("FAIL: stress pick\r\n");
                return 8;
            }

            if (!check_pattern(slots[slot], sizes[slot], seeds[slot])) {
                printf("FAIL: stress overlap\r\n");
                return 9;
            }

            free(slots[slot]);
            slots[slot] = NULL;
            sizes[slot] = 0;
            seeds[slot] = 0;
            active[slot] = 0;
            --active_count;
        }
    }

    for (i = 0; i < STRESS_SLOTS; ++i) {
        if (active[i]) {
            if (!check_pattern(slots[i], sizes[i], seeds[i])) {
                printf("FAIL: stress final\r\n");
                return 10;
            }
            free(slots[i]);
        }
    }

    printf("PASS: malloc\r\n");
    return 0;
}
