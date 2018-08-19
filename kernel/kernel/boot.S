.section ".text.boot"

.globl _start
_start:
    // Isolate core 0.
    mrs x4, mpidr_el1
    and x4, x4, #0x3
    cbnz x4, halt

    // Enable AArch64 in EL1.
    mrs x4, hcr_el2
    orr x4, x4, #(0x1 << 31)
    msr hcr_el2, x4

    // Change execution level to EL1h.
    mov x4, #0x3C5
    msr spsr_el2, x4
    adr x4, enter_el1
    msr elr_el2, x4
    eret

enter_el1:
    // Switch to EL1t.
    msr spsel, 0

    // Set stack pointer.
    ldr x4, =__start
    mov sp, x4

    // Do not trap SIMD/FP.
    mov x4, #(0x3 << 20)
    msr cpacr_el1, x4

    // Set up exception handlers.
    ldr x4, =vector_table
    msr vbar_el1, x4

    // Get the range of BSS.
    ldr x4, =__bss_start
    ldr x5, =__bss_end

loop_clear_bss:
    // Zero out BSS.
    stp xzr, xzr, [x4], #16
    cmp x4, x5
    blo loop_clear_bss

    bl kernel_main

.globl halt
halt:
    bl disable_interrupts
loop_halt:
    wfi
    b loop_halt
