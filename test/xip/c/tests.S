# test.S
.section .text
.global _start
_start:
    lui a0, 0x12345
    addi a0, a0, 0x678
loop:
    j loop
