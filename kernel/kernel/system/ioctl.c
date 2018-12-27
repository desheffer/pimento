#include <system.h>

registers_t* sys_ioctl(registers_t* regs)
{
    // @TODO
    regs->regs[0] = 0;
    return regs;
}
