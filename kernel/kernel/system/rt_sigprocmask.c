#include <system.h>

long sys_rt_sigprocmask(int how, const sigset_t * set, sigset_t * oldset, size_t sigsetsize)
{
    // @TODO
    (void) how;
    (void) set;
    (void) oldset;
    (void) sigsetsize;

    return 0;
}
