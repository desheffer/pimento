#include <system.h>

SYSCALL_DEFINE2(setpgid, pid_t, pid, pid_t, pgid)
{
    // @TODO
    (void) pid;
    (void) pgid;

    return 0;
}
