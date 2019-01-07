#include <assert.h>
#include <serial.h>
#include <system.h>

long sys_readv(int fd, const struct iovec * iov, int iovcnt)
{
    long ret = 0;

    failif(fd != 0);

    while (iovcnt--) {
        char * buf = (char *) iov->iov_base;
        size_t count = iov->iov_len;

        while (count--) {
            char c = serial_getc();

            if ((c >= 0x32 && c < 0x7F) || c == '\n') {
                // Printable characters and line feed
                *(buf++) = c;
                ++ret;

                if (c == '\n') {
                    break;
                }
            } else if (c == 0x7F) {
                // Backspace
                if (ret > 0) {
                    count += 2;
                    --ret;
                    --buf;

                    serial_putc('\b');
                    serial_putc(' ');
                    serial_putc('\b');
                }
            }
        }

        ++iov;
    }

    return ret;
}
