#include "asm/entry.h"

.macro interrupts_enable
    msr daifclr, #DAIF_I
.endm

.globl cpu_switch_to
cpu_switch_to:
    mov x9, sp
    stp x19, x20, [x0, #16 * 0]
    stp x21, x22, [x0, #16 * 1]
    stp x23, x24, [x0, #16 * 2]
    stp x25, x26, [x0, #16 * 3]
    stp x27, x28, [x0, #16 * 4]
    stp x29, x9, [x0, #16 * 5]
    str x30, [x0, #16 * 6]

    ldp x19, x20, [x1, #16 * 0]
    ldp x21, x22, [x1, #16 * 1]
    ldp x23, x24, [x1, #16 * 2]
    ldp x25, x26, [x1, #16 * 3]
    ldp x27, x28, [x1, #16 * 4]
    ldp x29, x9, [x1, #16 * 5]
    ldr x30, [x1, #16 * 6]
    mov sp, x9

    interrupts_enable
    ret

.globl fpsimd_save
fpsimd_save:
    stp q0, q1, [x0, #16 * 0]
    stp q2, q3, [x0, #16 * 2]
    stp q4, q5, [x0, #16 * 4]
    stp q6, q7, [x0, #16 * 6]
    stp q8, q9, [x0, #16 * 8]
    stp q10, q11, [x0, #16 * 10]
    stp q12, q13, [x0, #16 * 12]
    stp q14, q15, [x0, #16 * 14]
    stp q16, q17, [x0, #16 * 16]
    stp q18, q19, [x0, #16 * 18]
    stp q20, q21, [x0, #16 * 20]
    stp q22, q23, [x0, #16 * 22]
    stp q24, q25, [x0, #16 * 24]
    stp q26, q27, [x0, #16 * 26]
    stp q28, q29, [x0, #16 * 28]
    stp q30, q31, [x0, #16 * 30]!

    mrs x1, fpsr
    mrs x2, fpcr
    stp x1, x2, [x0, #16 * 2]

    ret

.globl fpsimd_load
fpsimd_load:
    ldp q0, q1, [x0, #16 * 0]
    ldp q2, q3, [x0, #16 * 2]
    ldp q4, q5, [x0, #16 * 4]
    ldp q6, q7, [x0, #16 * 6]
    ldp q8, q9, [x0, #16 * 8]
    ldp q10, q11, [x0, #16 * 10]
    ldp q12, q13, [x0, #16 * 12]
    ldp q14, q15, [x0, #16 * 14]
    ldp q16, q17, [x0, #16 * 16]
    ldp q18, q19, [x0, #16 * 18]
    ldp q20, q21, [x0, #16 * 20]
    ldp q22, q23, [x0, #16 * 22]
    ldp q24, q25, [x0, #16 * 24]
    ldp q26, q27, [x0, #16 * 26]
    ldp q28, q29, [x0, #16 * 28]
    ldp q30, q31, [x0, #16 * 30]!

    msr fpsr, x1
    msr fpcr, x2
    ldp x1, x2, [x0, #16 * 2]

    ret

.globl task_entry
task_entry:
    mov x0, x20
    blr x19
    b exit
