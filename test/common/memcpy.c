/* test/common/memcpy.c — word-aligned memcpy for the bare-metal model build.
 *
 * GCC emits libcalls to `memcpy` for non-trivial copies, including NNoM's
 * `nnom_memcpy(...)` macro expansions on the inference hot path (Conv2D
 * im2col / padding loops in nnom_local.c run roughly 9 MB through here per
 * mel_compact inference).  A naive byte-at-a-time memcpy is ~4x slower
 * than aligned word copies on Hazard3, so this matters.
 *
 * Strategy:
 *   1. Walk the prefix one byte at a time until the destination is 4-byte
 *      aligned.
 *   2. If the source then happens to also be 4-byte aligned, copy in
 *      32-bit chunks (the common case — NNoM activation buffers are
 *      placed at aligned offsets, and tile sizes are usually multiples
 *      of the channel count which is itself a multiple of 4).
 *   3. Fall back to byte copy for unaligned source or the trailing 1..3
 *      bytes.
 *
 * No `n == 0` early-out: the loops naturally handle that.  No `d == s`
 * special case: GCC emits memcpy only when it has reason to.
 */

#include <stddef.h>
#include <stdint.h>

void *memcpy(void *dest, const void *src, size_t n)
{
    unsigned char       *d = (unsigned char *)dest;
    const unsigned char *s = (const unsigned char *)src;

    /* Phase 1: byte-copy until d is 4-byte aligned (or we run out). */
    while (n && ((uintptr_t)d & 3u)) {
        *d++ = *s++;
        --n;
    }

    /* Phase 2: word-copy if s is also aligned. */
    if (((uintptr_t)s & 3u) == 0u) {
        uint32_t       *dw = (uint32_t *)(void *)d;
        const uint32_t *sw = (const uint32_t *)(const void *)s;
        while (n >= 4u) {
            *dw++ = *sw++;
            n -= 4u;
        }
        d = (unsigned char *)(void *)dw;
        s = (const unsigned char *)(const void *)sw;
    }

    /* Phase 3: byte-copy the tail (and the whole thing if s wasn't
     * aligned in phase 2). */
    while (n--) {
        *d++ = *s++;
    }

    return dest;
}
