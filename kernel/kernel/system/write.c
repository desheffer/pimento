#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t sys_write(int fd, const char* buf, size_t count)
{
    ssize_t ret = 0;

    failif(fd != 1 && fd != 2);

    while (count--) {
        serial_putc(*(buf++));
        ++ret;
    }

    return ret;
}
