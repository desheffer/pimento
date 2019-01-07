#include <system.h>

SYSCALL_DEFINE3(fcntl, int, fd, int, cmd, int, arg)
{
    // @TODO
    (void) fd;
    (void) cmd;
    (void) arg;

    return 0;
}
