#include <assert.h>
#include <panic.h>
#include <scheduler.h>
#include <serial.h>
#include <unistd.h>

void sys_exit(int /*status*/)
{
    Scheduler::instance()->stopProcess();

    while (true) {
        asm volatile("wfi");
    }
}

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
    (void*) sys_exit,
    (void*) sys_read,
    (void*) sys_write,
};
