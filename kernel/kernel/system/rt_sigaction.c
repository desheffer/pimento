#include <signal.h>
#include <system.h>

SYSCALL_DEFINE3(rt_sigaction, int, signum, const struct sigaction *, act, struct sigaction *, oldact)
{
    // @TODO
    (void) signum;
    (void) act;
    (void) oldact;

    return 0;
}
