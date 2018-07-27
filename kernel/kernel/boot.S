.section ".text.boot"

.globl _start
_start:
    // Isolate core 0.
    mrs x4, mpidr_el1
    and x4, x4, #3
    cbnz x4, halt

    // Get current EL.
    mrs x4, currentel
    and x4, x4, #12

    // Skip if we are already in EL1.
    cmp x4, #4
    beq enter_el1

    // Disable coprocessor traps.
    mov x4, #0x33FF
    msr cptr_el2, x4
    msr hstr_el2, xzr
    mov x4, #(3 << 20)
    msr cpacr_el1, x4

    // Enable A64 in EL1.
    mov x4, #(1 << 31)
    msr hcr_el2, x4

    // Set up exception handlers.
    adr x4, vector_table
    msr vbar_el1, x4

    // Set EL0 stack pointer.
    mov x4, 0x80000
    msr sp_el0, x4

    // Change execution level to EL1t.
    mov x4, #0x3C4
    msr spsr_el2, x4
    adr x4, enter_el1
    msr elr_el2, x4
    eret

enter_el1:
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
    bl kernel_main
    b panic
