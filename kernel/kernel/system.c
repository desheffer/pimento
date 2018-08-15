#include <assert.h>
#include <system.h>

static syscall_t* _calls[SYSCALL_COUNT];

void system_init()
{
    for (unsigned i = 0; i < SYSCALL_COUNT; ++i) {
        system_register_call(i, (syscall_t*) do_invalid);
    }

    system_register_call(__NR_exit, (syscall_t*) do_exit);
    system_register_call(__NR_read, (syscall_t*) do_read);
    system_register_call(__NR_write, (syscall_t*) do_write);
}

long system_handler(unsigned n, long a, long b, long c, long d, long e, long f)
{
    assert(n < SYSCALL_COUNT);

    return _calls[n](a, b, c, d, e, f);
}

void system_register_call(unsigned n, syscall_t* call)
{
    _calls[n] = call;
}
