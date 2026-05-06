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

    /* XIP audio playback buffer at fixed flash offset, read by
     * peris/xip/xip_sample_player.v.  Only populated when the test build
     * compiles test/build/playback_samples.c (XIP_PLAYBACK=1); otherwise
     * the section is empty and consumes no flash. */
    .playback_samples 0x80010000 : {
        KEEP(*(.playback_samples))
        . = ALIGN(4);
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

    /DISCARD/ : { *(.note.GNU-stack) *(.comment) }
}
