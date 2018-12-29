#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t sys_read(int fd, char* buf, size_t count)
{
    ssize_t ret = 0;

    failif(fd != 0);

    while (count--) {
        char c = serial_getc();
        *(buf++) = c;
        ++ret;

        if (c == '\n') {
            break;
        }
    }

    return ret;
}
