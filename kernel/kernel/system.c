#include <assert.h>
#include <system.h>

static syscall_t* _calls[__NR_syscalls] = {0};

void system_init()
{
    system_register_call(__NR_brk, (syscall_t*) do_brk);
    system_register_call(__NR_execve, (syscall_t*) do_execve);
    system_register_call(__NR_exit, (syscall_t*) do_exit);
    system_register_call(__NR_exit_group, (syscall_t*) do_exit_group);
    system_register_call(__NR_ioctl, (syscall_t*) do_ioctl);
    system_register_call(__NR_read, (syscall_t*) do_read);
    system_register_call(__NR_readv, (syscall_t*) do_readv);
    system_register_call(__NR_set_tid_address, (syscall_t*) do_set_tid_address);
    system_register_call(__NR_write, (syscall_t*) do_write);
    system_register_call(__NR_writev, (syscall_t*) do_writev);
}

registers_t* system_handler(registers_t* regs, unsigned n)
{
    assert(n < __NR_syscalls);

    if (_calls[n]) {
        return _calls[n](regs);
    }

    return do_invalid(regs);
}

void system_register_call(unsigned n, syscall_t* call)
{
    _calls[n] = call;
}
