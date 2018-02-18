.section ".text.boot"

.globl _start

_start:
    // Isolate core 0.
    mrs x4, mpidr_el1
    and x4, x4, #3
    cbnz x4, halt

    // Set up the stack.
    mov sp, #0x80000

    // Clear out bss.
    adr x4, __bss_start
    adr x5, __bss_end
loop_clear_bss:
    // Store zero at x4.
    stp xzr, xzr, [x4], #16

    // Loop until bss_end.
    cmp x4, x5
    blo loop_clear_bss

    // Call kernel_main.
    ldr x3, =kernel_main
    blr x3

halt:
    wfe
    b halt
