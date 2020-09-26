#include <abi.h>
#include <pimento.h>
#include <scheduler.h>
#include <task.h>

SYSCALL_DEFINE2(mmap, void *, addr, size_t, length)
{
    struct task * task = scheduler_current_task();

    // @TODO: Allow more than a single page to be allocated.
    if (addr == 0 && length == page_size()) {
        return (long) mm_context_stack_top(task->mm_context);
    }

    return -EINVAL;
}
