#include <system.h>

process_regs_t* do_brk(process_regs_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
    return regs;
}
