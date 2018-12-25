#include <system.h>

registers_t* do_brk(registers_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
    return regs;
}
