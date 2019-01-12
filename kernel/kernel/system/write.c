#include <assert.h>
#include <serial.h>
#include <system.h>

SYSCALL_DEFINE3(write, int, fd, const char *, buf, size_t, count)
{
    long ret = 0;

    failif(fd != 1 && fd != 2);

    while (count--) {
        serial_putc(*(buf++));
        ++ret;
    }

    return ret;
}
