.section .vectors
.global _reset_vector
_reset_vector:
    j _start

.section .text.init
.global _start
_start:
    /* Set up stack pointer to top of 128KB SRAM */
    lui sp, 0x20000
    
    /* Call main function */
    call main

    /* If main returns, loop forever */
    j .
