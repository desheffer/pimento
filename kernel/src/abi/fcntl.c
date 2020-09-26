#include <abi.h>
#include <bits/fcntl.h>
#include <pimento.h>

SYSCALL_DEFINE3(fcntl, int, fd, int, cmd, int, arg)
{
    // @TODO: Perform the `cmd`.
    if (cmd == F_DUPFD) {
        (void) fd;
        (void) arg;

        return 0;
    }

    return -EINVAL;
}
