/* init_loop.S */

.section .vectors           /* This section MUST come first and contains only the jump */
.global _reset_vector
_reset_vector:
    j _start                /* Hardware starts here at 0x00000000 */

.section .text.init
.global _start

_start:
    /* Set up a minimal stack pointer */
    lui sp, 0x20000         /* Stack at top of 128KB SRAM (0x20000) */

    j main_loop

main_loop:
    /* Infinite loop - waiting for debugger */
    addi a0, zero, 123
    addi a1, zero, 456
    j main_loop             /* Loop forever */

/*Trap handler, on interrupt just loop*/
.section .trap
trap_handler:
    wfi
    j trap_handler
