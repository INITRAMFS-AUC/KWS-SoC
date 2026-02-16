.section .vectors, "ax"
.global _reset_vector
_reset_vector:
    j _start

.section .text.init, "ax"
.global _start
_start:
    /* 1. Setup the Stack Pointer */
    la sp, _stack_top

    /* 2. Copy the .data section from Flash (LMA) to SRAM (VMA) */
    la a0, _sidata  /* Source: End of .text in Flash */
    la a1, _sdata   /* Destination: Start of .data in SRAM */
    la a2, _edata   /* End limit for SRAM .data */
    bge a1, a2, init_bss_prep /* If size is 0, skip copy */

copy_data_loop:
    lw t0, 0(a0)    /* Load word from Flash */
    sw t0, 0(a1)    /* Store word to SRAM */
    addi a0, a0, 4  /* Advance source pointer */
    addi a1, a1, 4  /* Advance destination pointer */
    blt a1, a2, copy_data_loop

init_bss_prep:
    /* 3. Zero out the .bss section in SRAM */
    la a0, _sbss    /* Start of .bss in SRAM */
    la a1, _ebss    /* End of .bss in SRAM */
    bge a0, a1, call_main /* If size is 0, skip zeroing */
    li t0, 0        /* Load 0 once */

zero_bss_loop:
    sw t0, 0(a0)    /* Write 0 to SRAM */
    addi a0, a0, 4  /* Advance pointer */
    blt a0, a1, zero_bss_loop

call_main:
    /* 4. Call the C main() function */
    call main

    /* 5. Trap if main ever returns */
inf_loop:
    j inf_loop
