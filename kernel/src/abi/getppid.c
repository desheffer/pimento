#include <abi.h>
#include <pimento.h>
#include <scheduler.h>

SYSCALL_DEFINE0(getppid)
{
    struct task * task = scheduler_current_task();

    return task->parent ? task->parent->pid : 0;
}
