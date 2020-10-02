#include "abi.h"
#include "pimento.h"

SYSCALL_DEFINE4(openat, int, dirfd, const char *, pathname, int, flags, unsigned, mode)
{
    (void) dirfd;
    (void) pathname;
    (void) flags;
    (void) mode;

    return -ENOSYS;
}
