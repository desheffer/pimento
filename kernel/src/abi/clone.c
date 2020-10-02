#include "abi.h"
#include "clone.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

SYSCALL_DEFINE6(clone, int, flags, void *, child_stack, void *, arg, pid_t *, ptid, void *, newtls, pid_t *, ctid)
{
    (void) flags;
    (void) child_stack;
    (void) arg;
    (void) ptid;
    (void) newtls;
    (void) ctid;

    struct task * old_task = scheduler_current_task();

    struct task * new_task = clone(old_task);

    scheduler_schedule();

    return new_task->pid;
}
