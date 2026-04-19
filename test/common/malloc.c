#include <stddef.h>
#include <stdint.h>

typedef struct block_header {
    size_t size;
    struct block_header *next;
    int free;
} block_header_t;

extern char _ebss;
extern char _stack_top;

#define ALIGNMENT 8u
#define STACK_GUARD_BYTES 256u

static block_header_t *free_list = NULL;
static uintptr_t heap_lo = 0;
static uintptr_t heap_hi = 0;

static uintptr_t align_up_uintptr(uintptr_t v, uintptr_t a) {
    return (v + (a - 1u)) & ~(a - 1u);
}

static void init_heap_once(void) {
    if (free_list != NULL) {
        return;
    }

    heap_lo = align_up_uintptr((uintptr_t)&_ebss, ALIGNMENT);
    heap_hi = (uintptr_t)&_stack_top;
    if (heap_hi > STACK_GUARD_BYTES) {
        heap_hi -= STACK_GUARD_BYTES;
    }
    heap_hi &= ~(uintptr_t)(ALIGNMENT - 1u);

    if (heap_hi <= heap_lo + sizeof(block_header_t)) {
        free_list = NULL;
        return;
    }

    free_list = (block_header_t *)heap_lo;
    free_list->size = heap_hi - heap_lo - sizeof(block_header_t);
    free_list->next = NULL;
    free_list->free = 1;
}

static void split_block(block_header_t *blk, size_t need) {
    size_t remain = blk->size - need;
    if (remain <= sizeof(block_header_t) + ALIGNMENT) {
        return;
    }

    block_header_t *new_blk =
        (block_header_t *)((uint8_t *)(blk + 1) + need);
    new_blk->size = remain - sizeof(block_header_t);
    new_blk->next = blk->next;
    new_blk->free = 1;

    blk->size = need;
    blk->next = new_blk;
}

static void coalesce_all(void) {
    block_header_t *blk = free_list;
    while (blk != NULL && blk->next != NULL) {
        uint8_t *blk_end = (uint8_t *)(blk + 1) + blk->size;
        if (blk->free && blk->next->free && blk_end == (uint8_t *)blk->next) {
            blk->size += sizeof(block_header_t) + blk->next->size;
            blk->next = blk->next->next;
        } else {
            blk = blk->next;
        }
    }
}

static int safe_size_mul(size_t a, size_t b, size_t *out) {
    size_t acc = 0;

    if (a == 0 || b == 0) {
        *out = 0;
        return 1;
    }

    while (b--) {
        if (acc > ((size_t)-1) - a) {
            return 0;
        }
        acc += a;
    }

    *out = acc;
    return 1;
}

void *malloc(size_t size) {
    block_header_t *blk;
    size_t need;

    if (size == 0) {
        return NULL;
    }

    init_heap_once();
    if (free_list == NULL) {
        return NULL;
    }

    need = (size + (ALIGNMENT - 1u)) & ~(ALIGNMENT - 1u);

    blk = free_list;
    while (blk != NULL) {
        if (blk->free && blk->size >= need) {
            split_block(blk, need);
            blk->free = 0;
            return (void *)(blk + 1);
        }
        blk = blk->next;
    }

    return NULL;
}

void free(void *ptr) {
    block_header_t *blk;

    if (ptr == NULL) {
        return;
    }

    blk = ((block_header_t *)ptr) - 1;
    blk->free = 1;
    coalesce_all();
}

void *calloc(size_t nmemb, size_t size) {
    uint8_t *p;
    size_t total;
    size_t i;

    if (nmemb == 0 || size == 0) {
        return malloc(0);
    }

    if (!safe_size_mul(nmemb, size, &total)) {
        return NULL;
    }

    p = (uint8_t *)malloc(total);
    if (p == NULL) {
        return NULL;
    }

    for (i = 0; i < total; ++i) {
        p[i] = 0;
    }
    return p;
}

void *realloc(void *ptr, size_t size) {
    block_header_t *blk;
    void *newp;
    size_t copy_n;
    size_t i;

    if (ptr == NULL) {
        return malloc(size);
    }
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    blk = ((block_header_t *)ptr) - 1;
    if (blk->size >= size) {
        return ptr;
    }

    newp = malloc(size);
    if (newp == NULL) {
        return NULL;
    }

    copy_n = blk->size;
    for (i = 0; i < copy_n; ++i) {
        ((uint8_t *)newp)[i] = ((const uint8_t *)ptr)[i];
    }

    free(ptr);
    return newp;
}