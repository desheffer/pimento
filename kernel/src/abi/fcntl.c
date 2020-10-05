#include "abi.h"
#include "pimento.h"

SYSCALL_DEFINE3(fcntl, int, fd, int, cmd, int, arg)
{
    (void) fd;
    (void) cmd;
    (void) arg;

    return 0;
}
