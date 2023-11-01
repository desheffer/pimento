.section ".text._entry"

.macro vector_entry label
    .balign 0x80
    b \label
.endm

.macro vector_entry_invalid level cause
    .balign 0x80
    mov x0, \level
    mov x1, \cause
    mrs x2, esr_el1
    mrs x3, far_el1
    b vector_invalid
.endm

.globl vector_table
.balign 0x800
vector_table:
    vector_entry_invalid {LEVEL_EL1T} {TYPE_SYNC}
    vector_entry_invalid {LEVEL_EL1T} {TYPE_IRQ}
    vector_entry_invalid {LEVEL_EL1T} {TYPE_FIQ}
    vector_entry_invalid {LEVEL_EL1T} {TYPE_SERROR}

    vector_entry_invalid {LEVEL_EL1H} {TYPE_SYNC}
    vector_entry_invalid {LEVEL_EL1H} {TYPE_IRQ}
    vector_entry_invalid {LEVEL_EL1H} {TYPE_FIQ}
    vector_entry_invalid {LEVEL_EL1H} {TYPE_SERROR}

    vector_entry_invalid {LEVEL_EL0} {TYPE_SYNC}
    vector_entry_invalid {LEVEL_EL0} {TYPE_IRQ}
    vector_entry_invalid {LEVEL_EL0} {TYPE_FIQ}
    vector_entry_invalid {LEVEL_EL0} {TYPE_SERROR}

    vector_entry_invalid {LEVEL_EL0_32} {TYPE_SYNC}
    vector_entry_invalid {LEVEL_EL0_32} {TYPE_IRQ}
    vector_entry_invalid {LEVEL_EL0_32} {TYPE_FIQ}
    vector_entry_invalid {LEVEL_EL0_32} {TYPE_SERROR}
