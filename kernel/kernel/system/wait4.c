#include <assert.h>
#include <errno.h>
#include <scheduler.h>
#include <system.h>
#include <sys/wait.h>

SYSCALL_DEFINE4(wait4, pid_t, pid, int *, wstatus, int, options, struct rusage *, rusage)
{
    // @TODO
    (void) wstatus;
    (void) options;
    (void) rusage;

    failif(pid < -1);
    failif(pid == 0);
    failif(rusage != 0);

    if (pid == -1) {
        struct process * parent = scheduler_current();
        struct process * child = scheduler_get_by_ppid(parent->pid);

        if (child == 0) {
            return -ECHILD;
        }

        // @TODO: Don't lock to one specific child process.
        pid = child->pid;
    }

    while (scheduler_get_by_pid(pid) != 0) {
        scheduler_context_switch();
    }

    return pid;
}
