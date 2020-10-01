.section .text
.global start

start:
    ldr sp, =0xfff0
    b kmain