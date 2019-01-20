#include <assert.h>
#include <scheduler.h>
#include <system.h>

SYSCALL_DEFINE4(wait4, pid_t, pid, int *, wstatus, int, options, struct rusage *, rusage)
{
    if (pid <= 0) {
        return -1;
    }

    failif(wstatus != 0);
    failif(options != 0);
    failif(rusage != 0);

    while (scheduler_get_pid(pid) != 0) {
        scheduler_context_switch();
    }

    return 0;
}
