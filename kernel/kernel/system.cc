#include <assert.h>
#include <system.h>

System* System::_instance = 0;

System::System()
{
    for (unsigned i = 0; i < SYSCALL_COUNT; ++i) {
        registerCall(i, (syscall_t*) do_invalid);
    }

    registerCall(__NR_exit, (syscall_t*) do_exit);
    registerCall(__NR_read, (syscall_t*) do_read);
    registerCall(__NR_write, (syscall_t*) do_write);
}

System::~System()
{
}

void System::init()
{
    assert(!_instance);

    _instance = new System();
}

long System::handle(unsigned n, long a, long b, long c, long d, long e, long f) const
{
    assert(n < SYSCALL_COUNT);

    return _calls[n](a, b, c, d, e, f);
}

void System::registerCall(unsigned n, syscall_t* call)
{
    _calls[n] = call;
}

long system_handler(unsigned n, long a, long b, long c, long d, long e, long f)
{
    return System::instance()->handle(n, a, b, c, d, e, f);
}
