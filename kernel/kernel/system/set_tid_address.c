#include <system.h>

registers_t* sys_set_tid_address(registers_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
    return regs;
}
