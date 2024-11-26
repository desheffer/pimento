.section .text._start

.global _start
_start:
    b main

.section .text

main:
    // Call `write`:
    mov x0, 1
    adr x1, user_string
    mov x2, 4
    mov x8, #64
    svc #0

    ldr x0, =100000000
loop:
    sub x0, x0, 1
    nop
    cbnz x0, loop
    b main

.section .rodata

user_string:
.ascii "<u1>"
