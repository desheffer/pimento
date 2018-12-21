#include <system.h>

void do_brk(process_regs_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
}
