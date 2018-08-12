#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t do_read(int fd, void* buf, size_t count)
{
    char* cbuf = (char*) buf;
    ssize_t ret = 0;

    assert(fd == 0);

    while (count--) {
        *(cbuf++) = Serial::getc();
        ++ret;
    }

    return ret;
}
