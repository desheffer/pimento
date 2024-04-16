.section .text

.global enter_el0
enter_el0:
    mov x9, {SPSR_EL1_INIT_EL0}
    msr spsr_el1, x9

    msr elr_el1, x0
    eret
