#include <system.h>

process_regs_t* do_set_tid_address(process_regs_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
    return regs;
}
