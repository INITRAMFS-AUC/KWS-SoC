.section .vectors, "ax"
.global _reset_vector
_reset_vector:
    j _start

.section .text.init, "ax"
.global _start
_start:
    /* 1. Setup the trap vector to catch exceptions/GDB breakpoints */
    la t0, trap_handler
    csrw mtvec, t0

    /* 2. Setup the Global Pointer (gp) with linker relaxations disabled */
.option push
.option norelax
    la gp, __global_pointer$
.option pop

    /* 3. Setup the Stack Pointer (sp) and enforce 16-byte alignment */
    la sp, _stack_top
    andi sp, sp, -16

    /* 4. Copy the .data section from Flash to SRAM */
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
    /* 5. Zero out the .bss section in SRAM */
    la a0, _sbss
    la a1, _ebss
    bge a0, a1, call_main
    li t0, 0

zero_bss_loop:
    sw t0, 0(a0)
    addi a0, a0, 4
    blt a0, a1, zero_bss_loop

call_main:
    /* 6. Call the C main() function */
    call main

    /* 7. Trap if main ever returns */
inf_loop:
    j inf_loop

/* Safe landing zone for exceptions and GDB breakpoints */
.global trap_handler
.align 4
trap_handler:
    j trap_handler
