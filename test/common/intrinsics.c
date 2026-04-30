/* test/common/intrinsics.c — minimal libgcc replacements.
 *
 * The riscv32-none-elf toolchain in our flake ships a single libgcc
 * variant built for `-march=rv32imafdc -mabi=ilp32d` (RVC + double-float
 * ABI; toolchain configured with `--disable-multilib`).  Linking that
 * libgcc against our soft-float (`-mabi=ilp32`) NNoM build fails with
 *
 *     ld: can't link double-float modules with soft-float modules
 *
 * Hazard3 has no F/D unit and our int8 KWS firmware contains zero
 * floating-point operations, so we don't need any of libgcc's actual
 * float helpers.  The only libgcc symbol that survives `--gc-sections`
 * in the model build is `__ashrdi3` (signed 64-bit arithmetic shift,
 * pulled in by NNoM's Conv2D requantisation:
 *   `(int64_t)acc * mult >> shift`).  Providing it here lets us:
 *
 *   - drop `-lgcc` from the link,
 *   - keep the soft-float `-mabi=ilp32` ABI everywhere (smaller stack
 *     frames than the 16-byte-aligned `ilp32d` workaround),
 *   - avoid lying to the compiler about the CPU's ISA.
 *
 * The implementation is the standard non-recursive split-32 approach so
 * GCC can't turn it back into a libcall to itself.
 */

#include <stdint.h>

typedef union {
    int64_t  i64;
    struct { uint32_t lo; int32_t hi; } w;   /* RV is little-endian */
} dword_split_t;

/* `used` + `noinline` keep this function alive across LTO: gcc's backend
 * synthesises __ashrdi3 calls late (during code-gen of NNoM's conv2d
 * requantisation), after LTO has otherwise pruned "unused" definitions.  */
__attribute__((used, noinline, externally_visible))
int64_t __ashrdi3(int64_t a, int b)
{
    dword_split_t in  = { .i64 = a };
    dword_split_t out;

    if (b == 0) {
        return a;
    } else if (b < 32) {
        /* Shift hi right (sign-preserving), bring carry-in from lo. */
        out.w.hi = in.w.hi >> b;
        out.w.lo = (in.w.lo >> b) | ((uint32_t)in.w.hi << (32 - b));
    } else {
        /* b >= 32: hi collapses to its sign, lo gets hi >> (b-32). */
        out.w.hi = in.w.hi >> 31;          /* arithmetic — fills with sign bit */
        out.w.lo = (uint32_t)(in.w.hi >> (b - 32));
    }
    return out.i64;
}
