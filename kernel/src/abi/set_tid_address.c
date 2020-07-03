#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE1(set_tid_address, int *, tidptr)
{
    struct task * task = scheduler_current_task();

    *tidptr = task->pid;

    return task->pid;
}
