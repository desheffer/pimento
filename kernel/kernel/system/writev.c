#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t sys_writev(int fd, const struct iovec* iov, int iovcnt)
{
    ssize_t ret = 0;

    failif(fd != 1 && fd != 2);

    while (iovcnt--) {
        char* buf = (char*) iov->iov_base;
        size_t count = iov->iov_len;

        while (count--) {
            serial_putc(*(buf++));
            ++ret;
        }

        ++iov;
    }

    return ret;
}
