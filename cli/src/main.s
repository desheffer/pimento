.section .text._start

.global _start
_start:
    b main

.section .text

main:
    // Call `write`:
    mov x0, 1
    adr x1, user_string
    mov x2, 17
    mov x8, #64
    svc #0

user_wait:
    wfi
    b user_wait

.section .rodata

user_string:
.ascii "<hello from cli>\n"
