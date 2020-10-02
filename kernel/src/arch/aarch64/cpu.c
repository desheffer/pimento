#include "asm/cpu.h"
#include "asm/head.h"
#include "asm/entry.h"
#include "binprm.h"
#include "mm_context.h"
#include "pimento.h"

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
struct cpu_context * cpu_context_create_binprm(struct binprm * binprm)
{
    struct registers * registers = ((struct registers *) mm_context_kernel_stack_init(binprm->mm_context)) - 1;

    registers->sp = (uint64_t) binprm->stack_init;
    registers->pc = (uint64_t) binprm->entry;
    registers->pstate = PSR_MODE_USER;

    struct cpu_context * cpu_context = kcalloc(sizeof(struct cpu_context));

    cpu_context->x[0] = (uint64_t) load_regs;
    cpu_context->sp = (uint64_t) registers;
    cpu_context->pc = (uint64_t) task_entry;

    return cpu_context;
}

/**
 * Create the CPU context to clone a user program.
 */
struct cpu_context * cpu_context_create_clone(struct cpu_context * old_cpu_context,
                                              struct mm_context * mm_context)
{
    struct registers * registers = ((struct registers *) mm_context_kernel_stack_init(mm_context)) - 1;

    registers->x[0] = 0;

    struct cpu_context * cpu_context = kcalloc(sizeof(struct cpu_context));

    memcpy(cpu_context, old_cpu_context, sizeof(cpu_context));

    cpu_context->x[0] = (uint64_t) load_regs;
    cpu_context->sp = (uint64_t) registers;
    cpu_context->pc = (uint64_t) task_entry;

    return cpu_context;
}

/**
 * Destroy a CPU context.
 */
void cpu_context_destroy(struct cpu_context * cpu_context)
{
    kfree(cpu_context);
}
