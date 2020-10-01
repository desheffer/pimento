#include <cpu_context.h>
#include <critical.h>
#include <mm_context.h>
#include <pimento.h>
#include <task.h>
#include <vfs_context.h>

static unsigned _next_pid = 1;

/**
 * Assign a new PID.
 */
static unsigned _assign_pid(void)
{
    critical_start();

    unsigned pid = _next_pid++;

    critical_end();

    return pid;
}

/**
 * Create a task representing the "init" process.
 */
struct task * task_create_init(void)
{
    // Create a new task control block.
    struct task * task = kcalloc(sizeof(struct task));

    // Assign basic information.
    task->state = new;
    strncpy(task->name, "init", sizeof(task->name));
    task->pid = _assign_pid();

    task->mm_context = mm_context_create_kernel();

    task->cpu_context = cpu_context_create_init();

    task->vfs_context = vfs_context_create();

    task->state = running;

    return task;
}

/**
 * Create a task to execute user code.
 */
struct task * task_create(unsigned pid, const char * name,
                          struct mm_context * mm_context,
                          struct cpu_context * cpu_context)
{
    // Create a new task control block.
    struct task * task = kcalloc(sizeof(struct task));

    // Assign basic information.
    task->state = new;
    strncpy(task->name, name, sizeof(task->name));

    if (pid == 0) {
        pid = _assign_pid();
    }
    task->pid = pid;

    task->mm_context = mm_context;

    task->cpu_context = cpu_context;

    task->vfs_context = vfs_context_create();

    return task;
}
