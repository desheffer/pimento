#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE2(mmap, void *, addr, size_t, length)
{
    struct task * task = scheduler_current_task();

    if (addr == 0 && length == 0) {
        return (long) mm_context_stack_top(task->mm_context);
    }

    return -EINVAL;
}
