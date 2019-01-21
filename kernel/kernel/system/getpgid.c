#include <system.h>

SYSCALL_DEFINE1(getpgid, pid_t, pid)
{
    // @TODO
    (void) pid;

    return 0;
}
