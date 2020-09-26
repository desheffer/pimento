#include <asm/cpu.h>
#include <asm/head.h>
#include <asm/entry.h>
#include <page.h>
#include <pimento.h>
#include <task.h>

/**
 * Create the CPU context for the "init" task.
 */
struct cpu_context * cpu_context_create_init(void)
{
    struct cpu_context * cpu_context = kcalloc(sizeof(struct cpu_context));

    return cpu_context;
}

/**
 * Create the CPU context to execute a user program.
 */
struct cpu_context * cpu_context_create_user(struct task * task,
                                             struct binprm * binprm)
{
    struct cpu_context * cpu_context = kcalloc(sizeof(struct cpu_context));

    struct page * page = mm_context_page_alloc(task->mm_context);

    struct registers * registers = (struct registers *) ((uint64_t) page->vaddr + page_size()) - 1;

    cpu_context->x[0] = (uint64_t) load_regs;
    cpu_context->sp = (uint64_t) registers;
    cpu_context->pc = (uint64_t) task_entry;

    registers->sp = (uint64_t) binprm->stack_bottom;
    registers->pc = (uint64_t) binprm->entry;
    registers->pstate = PSR_MODE_USER;

    return cpu_context;
}

/**
 * Destroy the CPU context for a task.
 */
void cpu_context_destroy(struct task * task)
{
    kfree(task->cpu_context);
}
