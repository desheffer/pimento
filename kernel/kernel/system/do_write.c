#include <assert.h>
#include <serial.h>
#include <system.h>

process_regs_t* do_write(process_regs_t* regs)
{
    int fd = (int) regs->regs[0];
    char* buf = (char*) regs->regs[1];
    size_t count = (size_t) regs->regs[2];

    ssize_t ret = 0;

    assert(fd == 1);

    while (count--) {
        serial_putc(*(buf++));
        ++ret;
    }

    regs->regs[0] = ret;
    return regs;
}
