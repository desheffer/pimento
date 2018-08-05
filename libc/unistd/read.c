#include <unistd.h>

ssize_t read(int fd, void* buf, size_t count)
{
    return SYSCALL3(read, fd, buf, count);
}
