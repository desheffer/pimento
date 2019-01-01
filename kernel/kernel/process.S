.globl process_create_tail_wrapper
process_create_tail_wrapper:
    mov x0, x19
    mov x1, x20
    b process_create_tail

.globl process_exec_tail_wrapper
process_exec_tail_wrapper:
    mov x0, x19
    mov x1, x20
    mov x2, x21
    b process_exec_tail
