OUTPUT_ARCH(riscv)
ENTRY(_start)

MEMORY {
    SRAM (rwx)  : ORIGIN = 0x00000000, LENGTH = SRAM_SIZE
    FLASH (rx)  : ORIGIN = 0x80000000, LENGTH = 16M
}

SECTIONS {
    .text : {
        KEEP(*(.vectors))
        *(.text.init)
        *(.text .text.*)
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
        . = ALIGN(4);
        _sidata = .;
    } > FLASH

    .data : AT(_sidata) {
        . = ALIGN(4);
        _sdata = .;

        /* Define the global pointer for RISC-V linker relaxations */
        PROVIDE(__global_pointer$ = . + 0x800);

        *(.data .data.*)
        *(.sdata .sdata.*)
        . = ALIGN(4);
        _edata = .;
    } > SRAM

    .bss : {
        . = ALIGN(4);
        _sbss = .;
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
        . = ALIGN(4);
        _ebss = .;
    } > SRAM

    /* Enforce 16-byte alignment for the stack top */
    PROVIDE(_stack_top = ORIGIN(SRAM) + LENGTH(SRAM) - 16);

    /* XIP sample playback: fixed-location section for 1-second audio clips.
     * Placed after firmware text/rodata so playback samples don't overlap.
     * RTL xip_sample_player reads from here when XIP_PLAYBACK=1.
     * This section is empty in normal builds — only populated when
     * playback_samples.c is compiled and linked. */
    .playback_samples 0x80010000 : {
        KEEP(*(.playback_samples))
        . = ALIGN(4);
    } > FLASH

    /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
