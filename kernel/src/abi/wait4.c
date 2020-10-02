#include <sys/resource.h>
#include <sys/wait.h>

#include "abi.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

SYSCALL_DEFINE4(wait4, pid_t, pid, int *, stat_addr, int, options, struct rusage *, ru)
{
    (void) stat_addr;
    (void) ru;

    struct task * parent = scheduler_current_task();
    struct task * zombie_task = 0;

    while (1) {
        unsigned count = scheduler_count_children(parent);

        if (count == 0 && (options & WNOHANG) != 0) {
            break;
        }

        zombie_task = scheduler_wait(parent, pid);

        if (zombie_task != 0 || (options & WNOHANG) != 0) {
            break;
        }

        scheduler_schedule();
    }

    return zombie_task ? zombie_task->pid : -1;
}
