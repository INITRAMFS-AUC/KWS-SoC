/* test/common/memset.c — word-aligned memset for the bare-metal model build.
 *
 * NNoM's nnom_memset clears scratch buffers many times per inference (init
 * accumulator rows, padding zero-fill in nnom_local.c).  Same alignment
 * strategy as test/common/memcpy.c: byte until aligned, word in the middle,
 * byte tail.  The fill byte is splatted into a 32-bit word once.
 */

#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t n)
{
    unsigned char *d = (unsigned char *)dest;
    unsigned char  v = (unsigned char)value;

    /* Phase 1: byte-fill until d is 4-byte aligned (or we run out). */
    while (n && ((uintptr_t)d & 3u)) {
        *d++ = v;
        --n;
    }

    /* Phase 2: 32-bit word-fill.  Splat v into all four bytes. */
    uint32_t        word = (uint32_t)v;
    word |= word << 8;
    word |= word << 16;
    uint32_t       *dw   = (uint32_t *)(void *)d;
    while (n >= 4u) {
        *dw++ = word;
        n -= 4u;
    }
    d = (unsigned char *)(void *)dw;

    /* Phase 3: byte-fill the tail. */
    while (n--) {
        *d++ = v;
    }

    return dest;
}
