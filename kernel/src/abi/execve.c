#include "abi.h"
#include "exec.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

SYSCALL_DEFINE3(execve, const char *, pathname, char * const *, argv, char * const *, envp)
{
    struct task * old_task = scheduler_current_task();

    // @TODO: Safe copy from user space.
    exec(old_task, pathname, argv, envp);

    scheduler_schedule();

    return -ENOENT;
}
