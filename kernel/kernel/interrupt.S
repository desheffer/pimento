.globl enable_interrupts
enable_interrupts:
    msr daifclr, #2
    ret

.globl disable_interrupts
disable_interrupts:
    msr daifset, #2
    ret
