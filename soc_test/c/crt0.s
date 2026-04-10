.section .vectors
.global _reset_vector
_reset_vector:
    j _start
    .fill 15, 4, 0   /* pad to 0x40 bytes so _start lands at RESET_VECTOR=0x40 */

.section .text.init
.global _start
_start:
    /* Zero all general-purpose registers.
     * RESET_REGFILE=0 in Hazard3 means registers are NOT cleared on reset,
     * so they retain garbage values from a previous run when using GDB reset. */
    li x1,  0
    li x2,  0
    li x3,  0
    li x4,  0
    li x5,  0
    li x6,  0
    li x7,  0
    li x8,  0
    li x9,  0
    li x10, 0
    li x11, 0
    li x12, 0
    li x13, 0
    li x14, 0
    li x15, 0
    li x16, 0
    li x17, 0
    li x18, 0
    li x19, 0
    li x20, 0
    li x21, 0
    li x22, 0
    li x23, 0
    li x24, 0
    li x25, 0
    li x26, 0
    li x27, 0
    li x28, 0
    li x29, 0
    li x30, 0
    li x31, 0

    /* Initialize the global pointer for gp-relative data access.
     * GCC uses gp-relative addressing for .rodata/.sdata/.sbss within 2KB of
     * __global_pointer$. Without this, string literals in uart_printf (and any
     * other .rodata access) would load from a garbage address, corrupting the
     * AHB bus or reading wrong data. */
.option push
.option norelax
    la gp, __global_pointer$
.option pop

    /* Set up stack pointer */
    li sp, 0x2000

    call main

    /* If main returns, loop forever */
    j .
