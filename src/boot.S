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
    mov x4, 0x8000
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

.macro save_regs
    // Save registers.
    stp x29, x30, [sp, #-16]!
    stp x27, x28, [sp, #-16]!
    stp x25, x26, [sp, #-16]!
    stp x23, x24, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
    stp x19, x20, [sp, #-16]!
    stp x17, x18, [sp, #-16]!
    stp x15, x16, [sp, #-16]!
    stp x13, x14, [sp, #-16]!
    stp x11, x12, [sp, #-16]!
    stp x9, x10, [sp, #-16]!
    stp x7, x8, [sp, #-16]!
    stp x5, x6, [sp, #-16]!
    stp x3, x4, [sp, #-16]!
    stp x1, x2, [sp, #-16]!

    // Save system registers.
    mrs x23, sp_el0
    mrs x22, elr_el1
    mrs x21, spsr_el1
    stp x23, x0, [sp, #-16]!
    stp x21, x22, [sp, #-16]!
.endm

.macro load_regs
    // Load system registers.
    ldp x21, x22, [sp], #16
    ldp x23, x0, [sp], #16
    msr spsr_el1, x21
    msr elr_el1, x22
    msr sp_el0, x23

    // Load registers.
    ldp x1, x2, [sp], #16
    ldp x3, x4, [sp], #16
    ldp x5, x6, [sp], #16
    ldp x7, x8, [sp], #16
    ldp x9, x10, [sp], #16
    ldp x11, x12, [sp], #16
    ldp x13, x14, [sp], #16
    ldp x15, x16, [sp], #16
    ldp x17, x18, [sp], #16
    ldp x19, x20, [sp], #16
    ldp x21, x22, [sp], #16
    ldp x23, x24, [sp], #16
    ldp x25, x26, [sp], #16
    ldp x27, x28, [sp], #16
    ldp x29, x30, [sp], #16
.endm

.balign 0x800
_vector_table:
    // Synchronous
    .balign 0x80
    save_regs
    mov x0, #0x11
    b asm_exception_handler

    // IRQ
    .balign 0x80
    save_regs
    mov x0, #0x12
    b asm_irq_handler

    // FIQ
    .balign 0x80
    save_regs
    mov x0, #0x14
    b asm_exception_handler

    // SError
    .balign 0x80
    save_regs
    mov x0, #0x18
    b asm_exception_handler

    // Synchronous
    .balign 0x80
    save_regs
    mov x0, #0x21
    b asm_exception_handler

    // IRQ
    .balign 0x80
    save_regs
    mov x0, #0x22
    b asm_exception_handler

    // FIQ
    .balign 0x80
    save_regs
    mov x0, #0x24
    b asm_exception_handler

    // SError
    .balign 0x80
    save_regs
    mov x0, #0x28
    b asm_exception_handler

    // Synchronous
    .balign 0x80
    save_regs
    mov x0, #0x41
    b asm_exception_handler

    // IRQ
    .balign 0x80
    save_regs
    mov x0, #0x42
    b asm_exception_handler

    // FIQ
    .balign 0x80
    save_regs
    mov x0, #0x44
    b asm_exception_handler

    // SError
    .balign 0x80
    save_regs
    mov x0, #0x48
    b asm_exception_handler

    // Synchronous
    .balign 0x80
    save_regs
    mov x0, #0x81
    b asm_exception_handler

    // IRQ
    .balign 0x80
    save_regs
    mov x0, #0x82
    b asm_exception_handler

    // FIQ
    .balign 0x80
    save_regs
    mov x0, #0x84
    b asm_exception_handler

    // SError
    .balign 0x80
    save_regs
    mov x0, #0x88
    b asm_exception_handler

asm_exception_handler:
    // Call exception_handler.
    mov x1, x0
    mov x0, sp
    mrs x2, esr_el1
    mrs x3, far_el1
    bl exception_handler

    load_regs
    eret

asm_irq_handler:
    // Call irq_handler.
    mov x0, sp
    bl irq_handler

    load_regs
    eret
