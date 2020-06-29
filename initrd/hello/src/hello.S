.globl _start
_start:
    mov x0, 1
    adr x1, msg
    mov x2, #22
    mov x8, #0x40
    svc #0
hang:
    nop
    b hang
msg:
    .asciz "Hello from /bin/hello\n"
