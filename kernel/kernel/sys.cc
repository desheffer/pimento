#include <assert.h>
#include <panic.h>
#include <serial.h>
#include <unistd.h>

ssize_t sys_read(int fd, void* buf, size_t count)
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

ssize_t sys_write(int fd, const void* buf, size_t count)
{
    const char* cbuf = (const char*) buf;
    ssize_t ret = 0;

    assert(fd == 1);

    while (count--) {
        Serial::putc(*(cbuf++));
        ++ret;
    }

    return ret;
}

void* syscall_table[] = {
    (void*) sys_read,
    (void*) sys_write,
};
