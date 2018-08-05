#include <unistd.h>

ssize_t write(int fd, const void* buf, size_t count)
{
    return SYSCALL3(write, fd, buf, count);
}
