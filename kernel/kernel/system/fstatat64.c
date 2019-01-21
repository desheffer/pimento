#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE4(fstatat64, int, dirfd, const char *, pathname, struct stat *, statbuf, int, flags)
{
    // @TODO
    (void) dirfd;
    (void) pathname;
    (void) statbuf;
    (void) flags;

    return -ENOENT;
}
