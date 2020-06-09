#include <asm/cpu.h>
#include <kstdlib.h>
#include <page.h>
#include <task.h>

/**
 * Create the CPU context for the "init" task.
 */
void cpu_context_create_init(struct task * task)
{
    task->cpu_context = kcalloc(sizeof(struct cpu_context));
}

/**
 * Create the CPU context to execute the given function.
 */
void cpu_context_create(struct task * task, task_function_t fn, void * data)
{
    task->cpu_context = kcalloc(sizeof(struct cpu_context));

    // @TODO: Ensure page is freed when task is finished.
    struct page * page = page_alloc();

    task->cpu_context->sp = (uint64_t) page->vaddr + page_size();
    task->cpu_context->pc = (uint64_t) task_entry;
    task->cpu_context->x[0] = (uint64_t) fn;
    task->cpu_context->x[1] = (uint64_t) data;
}

/**
 * Destroy the CPU context for a task.
 */
void cpu_context_destroy(struct task * task)
{
    kfree(task->cpu_context);
}
