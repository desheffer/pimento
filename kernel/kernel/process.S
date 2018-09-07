#include <process.h>

.globl move_to_user_mode
move_to_user_mode:
    bl disable_interrupts
    msr elr_el1, x0
    mov sp, x1
    ldr x2, =PSR_MODE_USER
    msr spsr_el1, x2
    eret
