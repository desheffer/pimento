.globl ttbr_switch_to
ttbr_switch_to:
    msr ttbr0_el1, x0
    isb
    ret
