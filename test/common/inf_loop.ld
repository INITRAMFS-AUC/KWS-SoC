OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
  /* Start execution here: the hardware reset vector */
  . = 0x00000000;

  /* Place the reset vector section first */
  .vectors : { *(.vectors) }

  /* The main code starts immediately after the vectors */
  .text :
  {
    *(.text.init)  /* Ensures _start goes here */
    *(.text)
    . = ALIGN(0x4);
  }

  .rodata : { *(.rodata) }
  .data : { *(.data) }
  .bss : { *(.bss) }

  /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
