OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  /* Start execution here: the hardware reset vector */
  . = 0x00000000;

  /* Place the reset vector section first, padded to 0x40 bytes */
  .vectors : {
    *(.vectors)
    . = 0x40;   /* guarantee _start is at RESET_VECTOR=0x40 */
  }

  /* The main code starts immediately after the vectors */
  .text :
  {
    *(.text.init)  /* Ensures _start goes here */
    *(.text)
    . = ALIGN(0x4);
  }

  .rodata : { *(.rodata) }

  /* __global_pointer$ must be defined for gp-relative relaxation.
   * Placed 0x800 bytes into .sdata so gp-relative offsets cover
   * the full ±2KB window around it. */
  .sdata : {
    __global_pointer$ = . + 0x800;
    *(.sdata*)
  }

  .data : { *(.data) }
  .sbss : { *(.sbss*) }
  .bss : { *(.bss) }

  /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
