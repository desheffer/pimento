#include <abi.h>
#include <pimento.h>

SYSCALL_DEFINE2(ioctl, int, fd, long unsigned, request)
{
    // @TODO
    (void) fd;
    (void) request;

    return 0;
}
