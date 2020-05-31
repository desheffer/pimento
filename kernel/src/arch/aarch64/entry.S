#include <asm/entry.h>

.macro interrupts_disable
    msr daifset, #DAIF_I
.endm

.macro interrupts_enable
    msr daifclr, #DAIF_I
.endm

.globl interrupts_enable
interrupts_enable:
    interrupts_enable
    ret

.globl interrupts_disable
interrupts_disable:
    interrupts_disable
    ret

.macro vector label
    .balign 0x80
    b \label
.endm

.globl vector_table
.balign 0x800
vector_table:
    vector el1_sync_invalid     // Synchronous EL1t
    vector el1_irq_invalid      // IRQ EL1t
    vector el1_fiq_invalid      // FIQ EL1t
    vector el1_error_invalid    // SError EL1t

    vector el1_sync             // Synchronous EL1h
    vector el1_irq              // IRQ EL1h
    vector el1_fiq_invalid      // FIQ EL1h
    vector el1_error_invalid    // SError EL1h

    vector el0_sync             // Synchronous 64-bit EL0
    vector el0_irq              // IRQ 64-bit EL0
    vector el0_fiq_invalid      // FIQ 64-bit EL0
    vector el0_error_invalid    // SError 64-bit EL0

    vector el0_sync_invalid     // Synchronous 32-bit EL0
    vector el0_irq_invalid      // IRQ 32-bit EL0
    vector el0_fiq_invalid      // FIQ 32-bit EL0
    vector el0_error_invalid    // SError 32-bit EL0

.macro save_regs
    sub sp, sp, #PROCESS_REGS_SIZE

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
    stp x28, x29, [sp, #16 * 14]

    mrs x21, sp_el0
    stp x30, x21, [sp, #16 * 15]

    mrs x21, elr_el1
    mrs x22, spsr_el1
    stp x21, x22, [sp, #16 * 16]
.endm

.macro load_regs
    ldp x21, x22, [sp, #16 * 16]
    msr elr_el1, x21
    msr spsr_el1, x22

    ldp x30, x21, [sp, #16 * 15]
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
    ldp x28, x29, [sp, #16 * 14]

    add sp, sp, #PROCESS_REGS_SIZE
.endm

.macro debug_entry reason
    mov x0, sp
    mov x1, \reason
    mrs x2, esr_el1
    mrs x3, far_el1
    bl debug_handler
.endm

el1_sync:
    save_regs
    debug_entry #BAD_SYNC
el1_sync_done:
    interrupts_disable
    load_regs
    eret

el1_irq:
    save_regs
    bl interrupts_handler
    load_regs
    eret

el0_sync:
    save_regs
    debug_entry #BAD_SYNC
el0_sync_done:
    interrupts_disable
    load_regs
    eret

el0_irq:
    save_regs
    bl interrupts_handler
    load_regs
    eret

.macro inv_entry el reason
    save_regs
    debug_entry \reason
    load_regs
    eret
.endm

el1_sync_invalid:
    inv_entry 1, #BAD_SYNC

el1_irq_invalid:
    inv_entry 1, #BAD_IRQ

el1_fiq_invalid:
    inv_entry 1, #BAD_FIQ

el1_error_invalid:
    inv_entry 1, #BAD_ERROR

el0_sync_invalid:
    inv_entry 0, #BAD_SYNC

el0_irq_invalid:
    inv_entry 0, #BAD_IRQ

el0_fiq_invalid:
    inv_entry 0, #BAD_FIQ

el0_error_invalid:
    inv_entry 0, #BAD_ERROR
