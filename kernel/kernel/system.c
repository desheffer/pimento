#include <assert.h>
#include <system.h>

#undef SYSCALL
#define SYSCALL(nr, call) [__NR_ ## nr] = call,

static syscall_t* _calls[__NR_syscalls] = {
#include <system/syscalls.h>
};

registers_t* system_handler(registers_t* regs, unsigned nr)
{
    assert(nr < __NR_syscalls);

    if (_calls[nr]) {
        return _calls[nr](regs);
    }

    return sys_invalid(regs);
}
