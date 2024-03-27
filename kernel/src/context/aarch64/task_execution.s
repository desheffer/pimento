.globl enter_el0
enter_el0:
    mov x9, {SPSR_EL1_INIT_EL0}
    msr spsr_el1, x9

    msr elr_el1, x0
    eret

// The instructions below implement the example user program.
.globl user_code_start
user_code_start:
    // Call `write`:
    mov x0, 1
    adr x1, string
    mov x2, 11
    mov x8, #64
    svc #0

    mov x9, #0x500000
wait:
    sub x9, x9, #1
    cbnz x9, wait
    b user_code_start

string:
.ascii "<user_code>"

.globl user_code_end
user_code_end:
