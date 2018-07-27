#include <unistd.h>

ssize_t write(int fd, const void* buf, size_t count)
{
    ssize_t ret;

    asm volatile(
        "mov x8, %1\n\t"
        "mov x0, %2\n\t"
        "mov x1, %3\n\t"
        "mov x2, %4\n\t"
        "svc #0\n\t"
        "mov %0, x0"
        : "=r" (ret)
        : "r" (SYS_WRITE), "r" (fd), "r" (buf), "r" (count)
        : "x8", "x0", "x1", "x2"
    );

    return ret;
}
