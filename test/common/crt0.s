.section .vectors, "ax"
.global _reset_vector
_reset_vector:
    j _start
    /* pad to 0x40 bytes — the linker script enforces `. = 0x40` so _start lands
     * at RESET_VECTOR=0x40 even without this fill, but being explicit is safer */
    .fill 15, 4, 0

.section .text.init, "ax"
.global _start
_start:
    /* 1. Zero all general-purpose registers.
     * RESET_REGFILE=0 in Hazard3 means registers are NOT cleared on reset.
     * They retain garbage values from a previous run when using GDB `monitor reset`.
     * x0 is hardwired zero; x2 (sp) and x3 (gp) are set explicitly below. */
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

    /* 2. Set up the trap vector to catch exceptions and GDB breakpoints */
    la t0, trap_handler
    csrw mtvec, t0

    /* 3. Set up the Global Pointer (gp) for gp-relative data access.
     * Use lui+addi (absolute relocations) rather than la (PC-relative auipc)
     * so that gp resolves to the correct SRAM address even when this code
     * runs from a different region (e.g. XIP flash with a SRAM-linked binary).
     * Relaxation must be disabled while loading gp itself. */
.option push
.option norelax
    lui  gp, %hi(__global_pointer$)
    addi gp, gp, %lo(__global_pointer$)
.option pop

    /* 4. Set up the Stack Pointer — absolute for the same reason as gp above */
    lui  sp, %hi(_stack_top)
    addi sp, sp, %lo(_stack_top)
    andi sp, sp, -16

    /* 5. Copy the .data section from its load address (Flash/SRAM LMA) to SRAM VMA */
    la a0, _sidata
    la a1, _sdata
    la a2, _edata
    bge a1, a2, init_bss_prep

copy_data_loop:
    lw t0, 0(a0)
    sw t0, 0(a1)
    addi a0, a0, 4
    addi a1, a1, 4
    blt a1, a2, copy_data_loop

init_bss_prep:
    /* 6. Zero the .bss section */
    la a0, _sbss
    la a1, _ebss
    bge a0, a1, call_main
    li t0, 0

zero_bss_loop:
    sw t0, 0(a0)
    addi a0, a0, 4
    blt a0, a1, zero_bss_loop

call_main:
    /* 7. Call C main() */
    call main

    /* 8. If main returns, spin forever */
inf_loop:
    j inf_loop

/* Trap handler — saves all registers, returns from trap.
 * For exceptions (mcause[31]=0): advances mepc by 4 to skip the faulting
 * instruction so execution can continue.
 * For interrupts (mcause[31]=1): returns to the interrupted PC unchanged. */
.global trap_handler
.align 4
trap_handler:
    addi sp, sp, -(32*4)

    sw   x1,   1*4(sp)
    /* x2 (sp) saved below after we know the pre-trap value */
    sw   x3,   3*4(sp)
    sw   x4,   4*4(sp)
    sw   x5,   5*4(sp)
    sw   x6,   6*4(sp)
    sw   x7,   7*4(sp)
    sw   x8,   8*4(sp)
    sw   x9,   9*4(sp)
    sw   x10, 10*4(sp)
    sw   x11, 11*4(sp)
    sw   x12, 12*4(sp)
    sw   x13, 13*4(sp)
    sw   x14, 14*4(sp)
    sw   x15, 15*4(sp)
    sw   x16, 16*4(sp)
    sw   x17, 17*4(sp)
    sw   x18, 18*4(sp)
    sw   x19, 19*4(sp)
    sw   x20, 20*4(sp)
    sw   x21, 21*4(sp)
    sw   x22, 22*4(sp)
    sw   x23, 23*4(sp)
    sw   x24, 24*4(sp)
    sw   x25, 25*4(sp)
    sw   x26, 26*4(sp)
    sw   x27, 27*4(sp)
    sw   x28, 28*4(sp)
    sw   x29, 29*4(sp)
    sw   x30, 30*4(sp)
    sw   x31, 31*4(sp)

    /* Save pre-trap sp */
    addi t0, sp, (32*4)
    sw   t0,  2*4(sp)

    /* Advance mepc by 4 only for synchronous exceptions (mcause[31] == 0) */
    csrr t0, mcause
    csrr t1, mepc
    bltz t0, .Lno_advance     /* negative → bit 31 set → interrupt, don't advance */
    addi t1, t1, 4
.Lno_advance:
    csrw mepc, t1

    lw   x1,   1*4(sp)
    lw   x3,   3*4(sp)
    lw   x4,   4*4(sp)
    lw   x5,   5*4(sp)
    lw   x6,   6*4(sp)
    lw   x7,   7*4(sp)
    lw   x8,   8*4(sp)
    lw   x9,   9*4(sp)
    lw   x10, 10*4(sp)
    lw   x11, 11*4(sp)
    lw   x12, 12*4(sp)
    lw   x13, 13*4(sp)
    lw   x14, 14*4(sp)
    lw   x15, 15*4(sp)
    lw   x16, 16*4(sp)
    lw   x17, 17*4(sp)
    lw   x18, 18*4(sp)
    lw   x19, 19*4(sp)
    lw   x20, 20*4(sp)
    lw   x21, 21*4(sp)
    lw   x22, 22*4(sp)
    lw   x23, 23*4(sp)
    lw   x24, 24*4(sp)
    lw   x25, 25*4(sp)
    lw   x26, 26*4(sp)
    lw   x27, 27*4(sp)
    lw   x28, 28*4(sp)
    lw   x29, 29*4(sp)
    lw   x30, 30*4(sp)
    lw   x31, 31*4(sp)

    addi sp, sp, (32*4)
    mret
