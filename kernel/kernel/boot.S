#include <asm/boot.h>
#include <asm/memory.h>

.section ".text.boot"

.globl _start
_start:
    // Isolate core 0.
    mrs x4, mpidr_el1
    and x4, x4, #MPIDR_AFF0_MASK
    cbnz x4, halt

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

    // Do not trap SIMD/FP.
    ldr x4, =CPACR_CP10
    msr cpacr_el1, x4
    isb

    // Set up memory attributes.
    ldr x4, =MAIR_EL1
    msr mair_el1, x4
    isb

    // Set TTBR0 and TTBR1.
    bl mm_init

    // Set TCR.
    ldr x4, =TCR_EL1
    msr tcr_el1, x4
    isb

    // Enable MMU.
    mrs x4, sctlr_el1
    orr x4, x4, #SCTLR_M
    msr sctlr_el1, x4
    isb

    // Jump to virtual memory.
    ldr x4, =post_enable_mmu
    br x4

post_enable_mmu:
    // Move stack pointer to virtual memory.
    // @TODO: Move it to KSTACK_TOP.
    mov x4, sp
    ldr x5, =VA_START
    orr x4, x4, x5
    mov sp, x4

    // Get the range of BSS.
    ldr x4, =__bss_start
    ldr x5, =__bss_end

loop_clear_bss:
    // Zero out BSS.
    stp xzr, xzr, [x4], #16
    cmp x4, x5
    blo loop_clear_bss

    // Set up exception vectors.
    ldr x4, =vector_table
    msr vbar_el1, x4
    isb

    bl kernel_main

.globl halt
halt:
    bl disable_interrupts
loop_halt:
    wfi
    b loop_halt
