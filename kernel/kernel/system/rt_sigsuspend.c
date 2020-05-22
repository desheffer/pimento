#include <scheduler.h>
#include <signal.h>
#include <system.h>

SYSCALL_DEFINE2(rt_sigsuspend, const sigset_t *, mask, size_t, sigsetsize)
{
    // @TODO
    (void) mask;
    (void) sigsetsize;

    scheduler_context_switch();

    return -1;
}
