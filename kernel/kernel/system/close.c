#include <system.h>

SYSCALL_DEFINE1(close, int, fd)
{
    // @TODO
    (void) fd;

    return 0;
}
