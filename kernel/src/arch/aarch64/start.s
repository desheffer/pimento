.section ".text._start"

.globl _start
_start:
    // Get the range of BSS.
    ldr x0, =__bss_start
    ldr x1, =__bss_end

    // Zero out BSS.
loop_clear_bss:
    stp xzr, xzr, [x0], #16
    cmp x0, x1
    blo loop_clear_bss

    // Enable AArch64 in EL1.
    ldr x0, ={HCR_EL2_RW}
    mrs x1, hcr_el2
    orr x0, x0, x1
    msr hcr_el2, x0
    isb

    // Change execution level to EL1h.
    ldr x0, ={SPSR_EL2_M_EL1H}
    msr spsr_el2, x0
    adr x0, enter_el1
    msr elr_el2, x0
    eret

enter_el1:
    // Set up initial stack pointer.
    ldr x0, =__stack_start
    mov sp, x0

    // Set up vector table.
    adr x0, vector_table
    msr vbar_el1, x0
    isb

    b kernel_init
