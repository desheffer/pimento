#include <asm/head.h>

.section ".text.boot"

.globl _start
_start:
    // Isolate core 0.
    mrs x4, mpidr_el1
    and x4, x4, #MPIDR_AFF0_MASK
    cbnz x4, hang

    // Enable AArch64 in EL1.
    mrs x4, hcr_el2
    orr x4, x4, #HCR_RW
    msr hcr_el2, x4
    isb

    // Change execution level to EL1h.
    ldr x4, =PSR_MODE_INIT
    msr spsr_el2, x4
    adr x4, enter_el1
    msr elr_el2, x4
    eret

enter_el1:
    // Set up initial stack pointer.
    adr x4, _start
    mov sp, x4

    // Get the range of BSS.
    adr x4, LD_BSS_START
    adr x5, LD_BSS_END

loop_clear_bss:
    // Zero out BSS.
    stp xzr, xzr, [x4], #16
    cmp x4, x5
    blo loop_clear_bss

    // Set up exception vectors.
    adr x4, vector_table
    msr vbar_el1, x4
    isb

    bl kernel_start

    bl kernel_main

.globl hang
hang:
    wfe
    wfi
    b hang
