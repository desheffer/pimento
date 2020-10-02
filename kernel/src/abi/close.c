#include "abi.h"
#include "pimento.h"

SYSCALL_DEFINE1(close, int, fd)
{
    (void) fd;

    return -ENOSYS;
}
