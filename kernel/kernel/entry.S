#include <entry.h>

.macro vector label
    .balign 0x80
    b \label
.endm

.macro save_regs
    sub sp, sp, #PROCESS_STATE_SIZE

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

    add sp, sp, #PROCESS_STATE_SIZE
.endm

.globl vector_table
.balign 0x800
vector_table:
    // EL1t:
    vector invalid_handler  // Synchronous
    vector el1t_irq         // IRQ
    vector invalid_handler  // FIQ
    vector invalid_handler  // SError

    // EL1h:
    vector invalid_handler  // Synchronous
    vector invalid_handler  // IRQ
    vector invalid_handler  // FIQ
    vector invalid_handler  // SError

    // EL0:
    vector el0_sync         // Synchronous
    vector el0_irq          // IRQ
    vector invalid_handler  // FIQ
    vector invalid_handler  // SError

    // EL0 32-bit:
    vector invalid_handler  // Synchronous
    vector invalid_handler  // IRQ
    vector invalid_handler  // FIQ
    vector invalid_handler  // SError

el0_sync:
    msr spsel, 0
    save_regs
    mrs x21, esr_el1
    lsr x22, x21, #ESR_ELx_EC_SHIFT
    cmp x22, #ESR_ELx_EC_SVC64
    beq el0_svc
    b invalid_handler
el0_svc:
    cmp x8, #SYSCALL_COUNT
    bge invalid_handler
    adr x23, syscall_table
    ldr x24, [x23, x8, lsl #3]
    blr x24
el0_done:
    str x0, [sp, #PROCESS_STATE_X0]
    load_regs
    eret

el0_irq:
    msr spsel, 0
    save_regs
    mov x0, sp
    bl irq_handler
    mov sp, x0
    load_regs
    eret

el1t_irq:
    msr spsel, 0
    save_regs
    mov x0, sp
    bl irq_handler
    mov sp, x0
    load_regs
    eret

invalid_handler:
    msr spsel, 0
    save_regs
    mov x0, sp
    mrs x1, esr_el1
    mrs x2, far_el1
    bl debug_process_state
    b halt
