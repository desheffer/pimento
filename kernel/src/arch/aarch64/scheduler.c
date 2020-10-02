#include "asm/cpu.h"
#include "asm/mm.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

/**
 * Perform a context switch between two tasks.
 */
void switch_to(struct task * prev, struct task * next)
{
    mm_switch_to(next->mm_context);

    fpsimd_save(&prev->cpu_context->fpsimd_context);
    fpsimd_load(&next->cpu_context->fpsimd_context);

    cpu_switch_to(prev->cpu_context, next->cpu_context);
}
