#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE1(getpgid, pid_t, pid)
{
    if (pid != 0) {
        return pid;
    }

    struct task * task = scheduler_current_task();

    return task->pid;
}
