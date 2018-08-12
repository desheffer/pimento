#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t do_write(int fd, const void* buf, size_t count)
{
    const char* cbuf = (const char*) buf;
    ssize_t ret = 0;

    assert(fd == 1);

    while (count--) {
        Serial::putc(*(cbuf++));
        ++ret;
    }

    return ret;
}
