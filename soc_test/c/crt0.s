.section .vectors
.global _reset_vector
_reset_vector:
    j _start

.section .text.init
.global _start
_start:
    /* Set up stack pointer to the one supplied by the linker */
    li sp, 0x2000
    call main

    /* If main returns, loop forever */
    j .
