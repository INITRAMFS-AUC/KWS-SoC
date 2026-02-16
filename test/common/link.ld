OUTPUT_ARCH(riscv)
ENTRY(_start)

/* Define your memory map based on ahbl_splitter */
MEMORY {
    SRAM (rwx)  : ORIGIN = 0x00000000, LENGTH = 128K
    FLASH (rx)  : ORIGIN = 0x80000000, LENGTH = 16M
}

SECTIONS {
    /* Code and read-only data live in Flash */
    .text : {
        KEEP(*(.vectors))
        *(.text.init)
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        /* Mark the end of the Flash data. We will use this symbol in assembly
           to know where to start copying from. */
        _sidata = .;
    } > FLASH

    /* Initialized data has its VMA in SRAM, but LMA in FLASH */
    .data : AT(_sidata) {
        . = ALIGN(4);
        _sdata = .;        /* Start of RAM data */
        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _edata = .;        /* End of RAM data */
    } > SRAM

    /* Uninitialized data lives in SRAM (doesn't take up space in Flash) */
    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* Automatically set the stack pointer to the very top of SRAM */
    PROVIDE(_stack_top = ORIGIN(SRAM) + LENGTH(SRAM));

    /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
