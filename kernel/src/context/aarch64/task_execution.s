.section .text

.global enter_el0
enter_el0:
    mov x9, {SPSR_EL1_INIT_EL0}
    msr spsr_el1, x9

    msr elr_el1, x0
    eret

.section .rodata

// The instructions below implement the example user program.
.global user_code_start
user_code_start:
    // Call `write`:
    mov x0, 1
    adr x1, user_string
    mov x2, 19
    mov x8, #64
    svc #0

user_wait:
    wfi
    b user_wait

user_string:
.ascii "<hello from a task>"

.global user_code_end
user_code_end:
