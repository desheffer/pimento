#include <entry.h>
#include <interrupt.h>
#include <kstdio.h>
#include <panic.h>

static int _in_debug_process_regs = 0;

void debug_process_regs(struct registers * state, long unsigned reason, long unsigned esr, long unsigned far)
{
    disable_interrupts();

    if (_in_debug_process_regs) {
        panic();
    }

    _in_debug_process_regs = 1;

    const char * type = "Unknown";
    const char * ec = "Unknown";
    const char * fs = "Unknown";

    kputs(
        "\n"
        "[41m[97m                          [0m\n"
        "[41m[97m     Kernel Exception     [0m\n"
        "[41m[97m                          [0m\n"
        "\n"
    );

    switch (reason) {
        case BAD_SYNC:  type = "Synchronous"; break;
        case BAD_IRQ:   type = "IRQ"; break;
        case BAD_FIQ:   type = "FIQ"; break;
        case BAD_ERROR: type = "SError"; break;
    }

    switch (ESR_ELx_EC(esr)) {
        case ESR_ELx_EC_ILL:      ec = "Illegal Execution state"; break;
        case ESR_ELx_EC_SVC64:    ec = "SVC instruction"; break;
        case ESR_ELx_EC_IABT_LOW: ec = "Instruction Abort, lower level"; break;
        case ESR_ELx_EC_IABT_CUR: ec = "Instruction Abort, same level"; break;
        case ESR_ELx_EC_PC_ALIGN: ec = "PC alignment fault"; break;
        case ESR_ELx_EC_DABT_LOW: ec = "Data Abort, lower level"; break;
        case ESR_ELx_EC_DABT_CUR: ec = "Data Abort, same level"; break;
        case ESR_ELx_EC_SP_ALIGN: ec = "SP alignment fault"; break;
        case ESR_ELx_EC_BRK64:    ec = "Breakpoint instruction"; break;
    }

    if (ESR_ELx_EC(esr) == ESR_ELx_EC_IABT_LOW
        || ESR_ELx_EC(esr) == ESR_ELx_EC_IABT_CUR
        || ESR_ELx_EC(esr) == ESR_ELx_EC_DABT_LOW
        || ESR_ELx_EC(esr) == ESR_ELx_EC_DABT_CUR
    ) {
        switch (ESR_ELx_xFSC(esr)) {
            case ESR_ELx_xFSC_ASF_0: fs = "Address size fault, level 0"; break;
            case ESR_ELx_xFSC_ASF_1: fs = "Address size fault, level 1"; break;
            case ESR_ELx_xFSC_ASF_2: fs = "Address size fault, level 2"; break;
            case ESR_ELx_xFSC_ASF_3: fs = "Address size fault, level 3"; break;
            case ESR_ELx_xFSC_TF_0:  fs = "Translation fault, level 0"; break;
            case ESR_ELx_xFSC_TF_1:  fs = "Translation fault, level 1"; break;
            case ESR_ELx_xFSC_TF_2:  fs = "Translation fault, level 2"; break;
            case ESR_ELx_xFSC_TF_3:  fs = "Translation fault, level 3"; break;
            case ESR_ELx_xFSC_AFF_1: fs = "Access flag fault, level 1"; break;
            case ESR_ELx_xFSC_AFF_2: fs = "Access flag fault, level 2"; break;
            case ESR_ELx_xFSC_AFF_3: fs = "Access flag fault, level 3"; break;
            case ESR_ELx_xFSC_PF_1:  fs = "Permission fault, level 1"; break;
            case ESR_ELx_xFSC_PF_2:  fs = "Permission fault, level 2"; break;
            case ESR_ELx_xFSC_PF_3:  fs = "Permission fault, level 3"; break;
        }
    }

    kprintf("Type = %s\n", type);
    kprintf("Class = %s (%#04x)\n", ec, (unsigned) ESR_ELx_EC(esr));
    kprintf("Fault Status = %s (%#04x)\n", fs, (unsigned) ESR_ELx_xFSC(esr));

    kprintf("\nRegisters:\n\n");

    kprintf("pstate = %08x%08x", (unsigned) (state->pstate >> 32), (unsigned) state->pstate);
    kprintf("   pc  = %08x%08x", (unsigned) (state->pc >> 32), (unsigned) state->pc);
    kprintf("\n");
    kprintf("   esr = %08x%08x", (unsigned) (esr >> 32), (unsigned) esr);
    kprintf("   far = %08x%08x", (unsigned) (far >> 32), (unsigned) far);
    kprintf("\n");

    for (unsigned i = 0; i < sizeof(state->regs) / sizeof(state->regs[0]); ++i) {
        if (i % 2 == 0) {
            kprintf("\n");
        }
        kprintf("   x%-2u = %08x%08x", i, (unsigned) (state->regs[i] >> 32), (unsigned) state->regs[i]);
    }
    kprintf("\n");

    halt();
}

void panic(void)
{
    disable_interrupts();

    kputs(
        "\n"
        "[41m[97m                      [0m\n"
        "[41m[97m     Kernel Panic     [0m\n"
        "[41m[97m                      [0m\n"
        "\n"
    );

    halt();
}
