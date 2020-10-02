#include "cpu_context.h"
#include "interrupts.h"
#include "mm_context.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"
#include "vfs_context.h"

/**
 * Clone a task.
 */
struct task * clone(struct task * old_task)
{
    struct mm_context * mm_context = mm_context_create_user_clone(old_task->mm_context);

    struct cpu_context * cpu_context = cpu_context_create_clone(old_task->cpu_context, mm_context);

    struct task * task = task_create(0, old_task->name, old_task, mm_context, cpu_context);

    vfs_context_copy(task->vfs_context, old_task->vfs_context);

    interrupts_disable();

    scheduler_enqueue(task);

    return task;
}
