.section ".text._start"

.globl _start
_start:
    // Get the range of BSS.
    ldr x9, =__bss_start
    ldr x10, =__bss_end

    // Zero out BSS.
loop_clear_bss:
    stp xzr, xzr, [x9], #16
    cmp x9, x10
    blo loop_clear_bss

    // Enable AArch64 in EL1.
    ldr x9, ={HCR_EL2_RW}
    mrs x10, hcr_el2
    orr x9, x9, x10
    msr hcr_el2, x9
    isb

    // Change execution level to EL1h.
    ldr x9, ={SPSR_EL2_INIT}
    msr spsr_el2, x9
    adr x9, enter_el1
    msr elr_el2, x9
    eret

enter_el1:
    // Set up initial stack pointer.
    ldr x9, =__stack_start
    mov sp, x9

    b kernel_init
