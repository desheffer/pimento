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

    // Set EL1 stack pointer.
    adr x4, _start
    msr sp_el1, x4

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
    adr x4, _vector_table
    msr vbar_el1, x4

    // Change execution level to EL1.
    mov x4, #0x3C4
    msr spsr_el2, x4
    adr x4, enter_el1
    msr elr_el2, x4
    eret

enter_el1:
    // Set up the stack.
    adr x4, _start
    mov sp, x4

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

.balign 0x800
_vector_table:
    // Synchronous
    .balign 0x80
    mov x0, #0
    mrs x1, esr_el1

    bl exception_handler

    // IRQ
    .balign 0x80
    b asm_irq_handler

    // FIQ
    .balign 0x80
    mov x0, #2
    mrs x1, esr_el1

    bl exception_handler

    // SError
    .balign 0x80
    mov x0, #3
    mrs x1, esr_el1

    bl exception_handler

asm_irq_handler:
    // Save registers to stack.
    stp x30, xzr, [sp, #-16]!
    stp x28, x29, [sp, #-16]!
    stp x26, x27, [sp, #-16]!
    stp x24, x25, [sp, #-16]!
    stp x22, x23, [sp, #-16]!
    stp x20, x21, [sp, #-16]!
    stp x18, x19, [sp, #-16]!
    stp x16, x17, [sp, #-16]!
    stp x14, x15, [sp, #-16]!
    stp x12, x13, [sp, #-16]!
    stp x10, x11, [sp, #-16]!
    stp x8, x9, [sp, #-16]!
    stp x6, x7, [sp, #-16]!
    stp x4, x5, [sp, #-16]!
    stp x2, x3, [sp, #-16]!
    stp x0, x1, [sp, #-16]!

    // Save system registers.
    mrs x0, spsr_el1
    mrs x1, elr_el1
    stp x0, x1, [sp, #-16]!

    // Save stack pointer to process_state.
    ldr x0, =process_state
    mov x1, sp
    str x1, [x0]

    bl irq_handler

    // Load stack pointer from process_state.
    ldr x0, =process_state
    ldr x1, [x0]
    mov sp, x1
    add x1, x1, #272
    msr sp_el0, x1

    // Restore system registers.
    ldp x0, x1, [sp], #16
    msr spsr_el1, x0
    msr elr_el1, x1

    // Restore registers from stack.
    ldp x0, x1, [sp], #16
    ldp x2, x3, [sp], #16
    ldp x4, x5, [sp], #16
    ldp x6, x7, [sp], #16
    ldp x8, x9, [sp], #16
    ldp x10, x11, [sp], #16
    ldp x12, x13, [sp], #16
    ldp x14, x15, [sp], #16
    ldp x16, x17, [sp], #16
    ldp x18, x19, [sp], #16
    ldp x20, x21, [sp], #16
    ldp x22, x23, [sp], #16
    ldp x24, x25, [sp], #16
    ldp x26, x27, [sp], #16
    ldp x28, x29, [sp], #16
    ldp x30, xzr, [sp], #16

    eret
