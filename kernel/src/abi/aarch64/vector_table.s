.section ".text._entry"

.macro vector_entry label
    .balign 0x80
    b \label
.endm

.macro vector_entry_invalid code
    .balign 0x80
    mov x0, \code
    b _vector_invalid
.endm

.globl vector_table
.balign 0x800
vector_table:
    // EL1t:
    vector_entry el1t_sync   // Synchronous
    vector_entry_invalid 0x1 // IRQ
    vector_entry_invalid 0x2 // FIQ
    vector_entry_invalid 0x3 // SError

    // EL1h:
    vector_entry el1h_sync   // Synchronous
    vector_entry el1h_irq    // IRQ
    vector_entry_invalid 0x6 // FIQ
    vector_entry_invalid 0x7 // SError

    // EL0:
    vector_entry el0_sync    // Synchronous
    vector_entry el0_irq     // IRQ
    vector_entry_invalid 0xA // FIQ
    vector_entry_invalid 0xB // SError

    // EL0 32-bit:
    vector_entry_invalid 0xC // Synchronous
    vector_entry_invalid 0xD // IRQ
    vector_entry_invalid 0xE // FIQ
    vector_entry_invalid 0xF // SError

.macro save_regs
    sub sp, sp, {TASK_REGS_SIZE}

    stp x0, x1, [sp, #16 * 0]
    stp x2, x3, [sp, #16 * 1]
    stp x4, x5, [sp, #16 * 2]
    stp x6, x7, [sp, #16 * 3]
    stp x8, x9, [sp, #16 * 4]
    stp x10, x11, [sp, #16 * 5]
    stp x12, x13, [sp, #16 * 6]
    stp x14, x15, [sp, #16 * 7]
    stp x16, x17, [sp, #16 * 8]
    stp x18, x19, [sp, #16 * 9]
    stp x20, x21, [sp, #16 * 10]
    stp x22, x23, [sp, #16 * 11]
    stp x24, x25, [sp, #16 * 12]
    stp x26, x27, [sp, #16 * 13]
    stp x28, fp, [sp, #16 * 14]

    mrs x21, sp_el0
    stp lr, x21, [sp, #16 * 15]

    mrs x21, elr_el1
    mrs x22, spsr_el1
    stp x21, x22, [sp, #16 * 16]
.endm

.macro load_regs
    ldp x21, x22, [sp, #16 * 16]
    msr elr_el1, x21
    msr spsr_el1, x22

    ldp lr, x21, [sp, #16 * 15]
    msr sp_el0, x21

    ldp x0, x1, [sp, #16 * 0]
    ldp x2, x3, [sp, #16 * 1]
    ldp x4, x5, [sp, #16 * 2]
    ldp x6, x7, [sp, #16 * 3]
    ldp x8, x9, [sp, #16 * 4]
    ldp x10, x11, [sp, #16 * 5]
    ldp x12, x13, [sp, #16 * 6]
    ldp x14, x15, [sp, #16 * 7]
    ldp x16, x17, [sp, #16 * 8]
    ldp x18, x19, [sp, #16 * 9]
    ldp x20, x21, [sp, #16 * 10]
    ldp x22, x23, [sp, #16 * 11]
    ldp x24, x25, [sp, #16 * 12]
    ldp x26, x27, [sp, #16 * 13]
    ldp x28, fp, [sp, #16 * 14]

    add sp, sp, {TASK_REGS_SIZE}
.endm

el1t_sync:
el1h_sync:
    save_regs
    mov x0, sp
    mrs x1, esr_el1
    mrs x2, far_el1
    bl _vector_sync_el1
    load_regs
    eret

el0_sync:
    save_regs
    mov x0, sp
    mrs x1, esr_el1
    mrs x2, far_el1
    bl _vector_sync_el0
    load_regs
    eret

el1h_irq:
el0_irq:
    save_regs
    bl _vector_irq
    load_regs
    eret
