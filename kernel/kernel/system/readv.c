#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t sys_readv(int fd, const struct iovec* iov, int iovcnt)
{
    ssize_t ret = 0;

    assert(fd == 0);

    while (iovcnt--) {
        char* buf = (char*) iov->iov_base;
        size_t count = iov->iov_len;

        while (count--) {
            char c = serial_getc();
            *(buf++) = c;
            ++ret;

            if (c == '\n') {
                break;
            }
        }

        ++iov;
    }

    return ret;
}
