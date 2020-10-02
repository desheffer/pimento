#include "abi.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

SYSCALL_DEFINE0(gettid)
{
    struct task * task = scheduler_current_task();

    return task->pid;
}
