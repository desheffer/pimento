#include <assert.h>
#include <scheduler.h>
#include <system.h>

void sys_wait4(pid_t pid, int * wstatus, int options, struct rusage * rusage)
{
    failif(pid <= 0);
    failif(wstatus != 0);
    failif(options != 0);
    failif(rusage != 0);

    while (scheduler_get_pid(pid) != 0) {
        scheduler_context_switch();
    }
}
