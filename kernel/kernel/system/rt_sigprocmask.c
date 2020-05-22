#include <signal.h>
#include <system.h>

SYSCALL_DEFINE4(rt_sigprocmask, int, how, const sigset_t *, set, sigset_t *, oldset, size_t, sigsetsize)
{
    // @TODO
    (void) how;
    (void) set;
    (void) oldset;
    (void) sigsetsize;

    return 0;
}
