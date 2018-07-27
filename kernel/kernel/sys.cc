#include <assert.h>
#include <panic.h>
#include <serial.h>
#include <unistd.h>

size_t sys_write(unsigned fd, const char* s, size_t len)
{
    size_t ret = 0;

    assert(fd == 1);

    while (len--) {
        Serial::putc(*(s++));
        ++ret;
    }

    return ret;
}

void* syscall_table[] = {
    (void*) sys_write,
};
