.section ".text._start"

.globl _start
_start:
    // Get the range of BSS.
    ldr x0, =__bss_start
    ldr x1, =__bss_end

    // Zero out BSS.
loop_clear_bss:
    stp xzr, xzr, [x0], #16
    cmp x0, x1
    blo loop_clear_bss

    // Set up initial stack pointer.
    ldr x0, =__stack_start
    mov sp, x0

    b kernel_init
