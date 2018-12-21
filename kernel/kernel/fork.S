#include <process.h>

.globl move_to_user_mode
move_to_user_mode:
    bl disable_interrupts
    msr elr_el1, x0
    mov sp, x1
    ldr x2, =PSR_MODE_USER
    msr spsr_el1, x2

    mov x0, #0          // spacer
    str x0, [sp, #-8]!
    mov x0, #0          // envp[0]
    str x0, [sp, #-8]!
    mov x0, #0          // argv[0]
    str x0, [sp, #-8]!
    mov x0, #0          // argc
    str x0, [sp, #-8]!

    eret
