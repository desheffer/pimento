#include <assert.h>
#include <serial.h>
#include <system.h>

ssize_t sys_read(int fd, char* buf, size_t count)
{
    ssize_t ret = 0;

    failif(fd != 0);

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

    return ret;
}
