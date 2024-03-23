.section ".text._start"

.globl _start
_start:
    // Get the range of BSS.
    ldr x9, =__bss_start
    ldr x10, =__bss_end
    and x9, x9, ~{VA_START}
    and x10, x10, ~{VA_START}

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

    // Initialize SCTLR_EL1.
    ldr x9, ={SCTLR_EL1_INIT}
    msr sctlr_el1, x9
    isb

    // Change exception level to EL1h.
    ldr x9, ={SPSR_EL2_INIT}
    msr spsr_el2, x9
    adr x9, enter_el1
    msr elr_el2, x9
    eret

enter_el1:
    // Set up initial stack pointer.
    ldr x9, =__stack_start
    and x9, x9, ~{VA_START}
    mov sp, x9

    // Enable virtual memory.
    bl virtual_memory_early_init

    // Relocate stack pointer to kernel virtual memory.
    mov x9, sp
    orr x9, x9, {VA_START}
    mov sp, x9

    // Relocate program counter to kernel virtual memory.
    adr x9, after_relocate
    orr x9, x9, {VA_START}
    br x9

after_relocate:
    b kernel_init
