#include <asm/cpu.h>
#include <scheduler.h>
#include <task.h>

/**
 * Perform a context switch between two tasks.
 */
void switch_to(struct task * prev, struct task * next)
{
    cpu_switch_to(prev->cpu_context, next->cpu_context);
}
