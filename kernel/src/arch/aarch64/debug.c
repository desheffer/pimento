#include <asm/debug.h>
#include <asm/entry.h>
#include <asm/head.h>
#include <interrupts.h>
#include <kstdio.h>
#include <messages.h>
#include <stdint.h>

/**
 * Output debug information when an unhandled exception occurs.
 */
void debug_handler(struct registers * regs, uint64_t reason, uint64_t esr, uint64_t far)
{
    interrupts_disable();

    kputs(EXCEPTION_MSG);

    kprintf("Reason = (%#04lx) ", reason);
    switch (reason) {
        case BAD_SYNC:  kputs("Synchronous"); break;
        case BAD_IRQ:   kputs("IRQ");         break;
        case BAD_FIQ:   kputs("FIQ");         break;
        case BAD_ERROR: kputs("SError");      break;
        default:        kputs("Unknown");
    }
    kputs("\n");

    kprintf("Class = (%#04lx) ", ESR_ELx_EC(esr));
    switch (ESR_ELx_EC(esr)) {
        case ESR_ELx_EC_ILL:      kputs("Illegal Execution state"); break;
        case ESR_ELx_EC_SVC64:    kputs("SVC instruction"); break;
        case ESR_ELx_EC_IABT_LOW: kputs("Instruction Abort, lower level"); break;
        case ESR_ELx_EC_IABT_CUR: kputs("Instruction Abort, same level"); break;
        case ESR_ELx_EC_PC_ALIGN: kputs("PC alignment fault"); break;
        case ESR_ELx_EC_DABT_LOW: kputs("Data Abort, lower level"); break;
        case ESR_ELx_EC_DABT_CUR: kputs("Data Abort, same level"); break;
        case ESR_ELx_EC_SP_ALIGN: kputs("SP alignment fault"); break;
        case ESR_ELx_EC_BRK64:    kputs("Breakpoint instruction"); break;
        default:                  kputs("Unknown"); break;
    }
    kputs("\n");

    if (ESR_ELx_EC(esr) == ESR_ELx_EC_IABT_LOW
        || ESR_ELx_EC(esr) == ESR_ELx_EC_IABT_CUR
        || ESR_ELx_EC(esr) == ESR_ELx_EC_DABT_LOW
        || ESR_ELx_EC(esr) == ESR_ELx_EC_DABT_CUR
    ) {
        kprintf("Fault Status = (%#04lx) ", ESR_ELx_xFSC(esr));
        switch (ESR_ELx_xFSC(esr)) {
            case ESR_ELx_xFSC_ASF_0: kputs("Address size fault, level 0"); break;
            case ESR_ELx_xFSC_ASF_1: kputs("Address size fault, level 1"); break;
            case ESR_ELx_xFSC_ASF_2: kputs("Address size fault, level 2"); break;
            case ESR_ELx_xFSC_ASF_3: kputs("Address size fault, level 3"); break;
            case ESR_ELx_xFSC_TF_0:  kputs("Translation fault, level 0");  break;
            case ESR_ELx_xFSC_TF_1:  kputs("Translation fault, level 1");  break;
            case ESR_ELx_xFSC_TF_2:  kputs("Translation fault, level 2");  break;
            case ESR_ELx_xFSC_TF_3:  kputs("Translation fault, level 3");  break;
            case ESR_ELx_xFSC_AFF_1: kputs("Access flag fault, level 1");  break;
            case ESR_ELx_xFSC_AFF_2: kputs("Access flag fault, level 2");  break;
            case ESR_ELx_xFSC_AFF_3: kputs("Access flag fault, level 3");  break;
            case ESR_ELx_xFSC_PF_1:  kputs("Permission fault, level 1");   break;
            case ESR_ELx_xFSC_PF_2:  kputs("Permission fault, level 2");   break;
            case ESR_ELx_xFSC_PF_3:  kputs("Permission fault, level 3");   break;
            default:                 kputs("Unknown"); break;
        }
        kputs("\n");
    }

    kputs("\n");
    kprintf("pstate = %016lx", regs->pstate);
    kprintf("   pc  = %016lx", regs->pc);
    kputs("\n");
    kprintf("   esr = %016lx", esr);
    kprintf("   far = %016lx", far);
    kputs("\n");

    for (unsigned i = 0; i < sizeof(regs->x) / sizeof(regs->x[0]); ++i) {
        if (i % 2 == 0) {
            kprintf("\n");
        }
        kprintf("   x%-2u = %016lx", i, regs->x[i]);
    }
    kputs("\n");

    hang();
}
