#include <abi.h>
#include <mm_context.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE1(brk, void *, addr)
{
    struct task * task = scheduler_current_task();

    return (long) mm_context_brk(task->mm_context, addr);
}
