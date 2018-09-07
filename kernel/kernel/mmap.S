.globl switch_ttbr
switch_ttbr:
    msr ttbr0_el1, x0
    ic iallu
    dsb nsh
    isb
    ret
