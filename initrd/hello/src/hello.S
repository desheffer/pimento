.globl _start
_start:
    adr x0, msg
    mov x1, #22
    mov x8, #0x40
    svc #0
hang:
    nop
    b hang
msg:
    .asciz "Hello from /bin/hello\n"
