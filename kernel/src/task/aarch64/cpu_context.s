.globl cpu_context_switch
cpu_context_switch:
    // Save registers to `prev`.
    mov x9, sp
    stp x19, x20, [x0, #16 * 0]
    stp x21, x22, [x0, #16 * 1]
    stp x23, x24, [x0, #16 * 2]
    stp x25, x26, [x0, #16 * 3]
    stp x27, x28, [x0, #16 * 4]
    stp x29, x30, [x0, #16 * 5]
    str x9, [x0, #16 * 6]

    // Load registers from `next`.
    ldp x19, x20, [x1, #16 * 0]
    ldp x21, x22, [x1, #16 * 1]
    ldp x23, x24, [x1, #16 * 2]
    ldp x25, x26, [x1, #16 * 3]
    ldp x27, x28, [x1, #16 * 4]
    ldp x29, x30, [x1, #16 * 5]
    ldr x9, [x1, #16 * 6]
    mov sp, x9

    // Call `after(data)`.
    mov x0, x3
    br x2

.globl task_entry
task_entry:
    mov x0, x20
    blr x19
    b task_exit
