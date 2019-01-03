#include <assert.h>
#include <system.h>

#undef SYSCALL
#define SYSCALL(nr, call) [__NR_ ## nr] = (void*) call,

static syscall_t* _calls[__NR_syscalls] = {
#include <system/syscalls.h>
};

void system_handler(registers_t* regs, unsigned nr)
{
    assert(nr < __NR_syscalls);

    if (_calls[nr] != 0) {
        long unsigned ret = _calls[nr](
            regs->regs[0],
            regs->regs[1],
            regs->regs[2],
            regs->regs[3],
            regs->regs[4],
            regs->regs[5]
        );

        regs->regs[0] = ret;
    } else {
        sys_ni(nr);
    }
}
