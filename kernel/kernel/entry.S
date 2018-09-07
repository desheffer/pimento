#include <entry.h>
#include <memory.h>

.macro vector label
    .balign 0x80
    b \label
.endm

.globl vector_table
.balign 0x800
vector_table:
    vector el1_sync             // Synchronous EL1t
    vector el1_irq              // IRQ EL1t
    vector el1_fiq_invalid      // FIQ EL1t
    vector el1_error_invalid    // SError EL1t

    vector el1_sync_invalid     // Synchronous EL1h
    vector el1_irq_invalid      // IRQ EL1h
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
    str x30, [sp, #16 * 15]

    mrs x22, elr_el1
    mrs x21, spsr_el1
    stp x21, x22, [sp, #16 * 16]
.endm

.macro load_regs
    ldp x21, x22, [sp, #16 * 16]
    msr spsr_el1, x21
    msr elr_el1, x22

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
    ldr x30, [sp, #16 * 15]

    add sp, sp, #PROCESS_REGS_SIZE
.endm

.macro irq_stack_entry
    ldr x21, =(VA_START | 0x10000)
    mov sp, x21
.endm

.macro debug_entry reason
    mov x0, sp
    mov x1, \reason
    mrs x2, esr_el1
    mrs x3, far_el1
    bl debug_process_regs
.endm

el1_sync:
    msr spsel, 0
    save_regs
    bl enable_interrupts
    mrs x21, esr_el1
    and x21, x21, #ESR_ELx_EC_MASK
    lsr x21, x21, #ESR_ELx_EC_SHIFT
    cmp x21, #ESR_ELx_EC_DABT_CUR
    beq el1_sync_dabt
    debug_entry #BAD_SYNC
    b el1_sync_done
el1_sync_dabt:
    mrs x0, far_el1
    bl data_abort_handler
    b el1_sync_done
el1_sync_done:
    bl disable_interrupts
    load_regs
    eret

el1_irq:
    msr spsel, 0
    save_regs
    mov x0, sp
    irq_stack_entry
    bl interrupt_handler
    mov sp, x0
    load_regs
    eret

el0_sync:
    msr spsel, 0
    save_regs
    bl enable_interrupts
    mrs x21, esr_el1
    and x21, x21, #ESR_ELx_EC_MASK
    lsr x21, x21, #ESR_ELx_EC_SHIFT
    cmp x21, #ESR_ELx_EC_SVC64
    beq el0_sync_svc
    cmp x21, #ESR_ELx_EC_DABT_LOW
    beq el0_sync_dabt
    debug_entry #BAD_SYNC
    b el0_sync_done
el0_sync_svc:
    mov x0, sp
    mov x1, x8
    bl system_handler
    b el0_sync_done
el0_sync_dabt:
    mrs x0, far_el1
    bl data_abort_handler
    b el0_sync_done
el0_sync_done:
    bl disable_interrupts
    load_regs
    eret

el0_irq:
    msr spsel, 0
    save_regs
    mov x0, sp
    irq_stack_entry
    bl interrupt_handler
    mov sp, x0
    load_regs
    eret

.macro inv_entry reason
    msr spsel, 0
    save_regs
    debug_entry \reason
    load_regs
    eret
.endm

el1_sync_invalid:
    inv_entry #BAD_SYNC

el1_irq_invalid:
    inv_entry #BAD_IRQ

el1_fiq_invalid:
    inv_entry #BAD_FIQ

el1_error_invalid:
    inv_entry #BAD_ERROR

el0_sync_invalid:
    inv_entry #BAD_SYNC

el0_irq_invalid:
    inv_entry #BAD_IRQ

el0_fiq_invalid:
    inv_entry #BAD_FIQ

el0_error_invalid:
    inv_entry #BAD_ERROR

.globl enable_interrupts
enable_interrupts:
    msr daifclr, #2
    ret

.globl disable_interrupts
disable_interrupts:
    msr daifset, #2
    ret
