#include <assert.h>
#include <serial.h>
#include <service.h>

Service* Service::_instance = 0;

Service::Service()
{
}

Service::~Service()
{
}

void Service::init()
{
    assert(!_instance);

    _instance = new Service();
}

Service* Service::instance() {
    assert(_instance);

    return _instance;
}

void Service::handle(process_state_t* state)
{
    // Source: https://thog.github.io/syscalls-table-aarch64/latest.html
    if (state->x[8] == 0x40) {
        // write

        unsigned fd = state->x[0];
        const char* s = (const char*) state->x[1];
        size_t len = state->x[2];

        assert(fd == 1);

        while (len--) {
            Serial::putc(*(s--));
        }
    }
}
