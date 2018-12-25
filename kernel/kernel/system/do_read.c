#include <assert.h>
#include <serial.h>
#include <system.h>

registers_t* do_read(registers_t* regs)
{
    int fd = (int) regs->regs[0];
    char* buf = (char*) regs->regs[1];
    size_t count = (size_t) regs->regs[2];

    ssize_t ret = 0;

    assert(fd == 0);

    while (count--) {
        char c = serial_getc();
        *(buf++) = c;
        ++ret;

        if (c == '\n') {
            break;
        }
    }

    regs->regs[0] = ret;
    return regs;
}
