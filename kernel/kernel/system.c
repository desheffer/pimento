#include <assert.h>
#include <system.h>

static syscall_t* _calls[SYSCALL_COUNT] = {0};

void system_init()
{
    system_register_call(__NR_exit, (syscall_t*) do_exit);
    system_register_call(__NR_read, (syscall_t*) do_read);
    system_register_call(__NR_write, (syscall_t*) do_write);
}

void system_handler(unsigned n, process_regs_t* regs)
{
    assert(n < SYSCALL_COUNT);

    if (_calls[n]) {
        _calls[n](regs);
    } else {
        do_invalid(regs);
    }
}

void system_register_call(unsigned n, syscall_t* call)
{
    _calls[n] = call;
}
