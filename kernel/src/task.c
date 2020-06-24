#include <critical.h>
#include <pimento.h>
#include <task.h>

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

    task->state = running;

    return task;
}

/**
 * Create a task to execute the given function.
 */
struct task * task_create(const char * name, task_function_t fn, void * data)
{
    // Create a new task control block.
    struct task * task = kcalloc(sizeof(struct task));

    // Assign basic information.
    task->state = new;
    strncpy(task->name, name, sizeof(task->name));
    task->pid = _assign_pid();

    task->mm_context = mm_context_create_kernel();

    task->cpu_context = cpu_context_create(task, fn, data);

    return task;
}

/**
 * Create a task to execute user code.
 */
struct task * task_create_binprm(const char * name, struct binprm * binprm,
                                 unsigned pid)
{
    // Create a new task control block.
    struct task * task = kcalloc(sizeof(struct task));

    // Assign basic information.
    task->state = new;
    strncpy(task->name, name, sizeof(task->name));

    if (pid == 0) {
        task->pid = _next_pid++;
    }

    task->mm_context = binprm->mm_context;

    task->cpu_context = cpu_context_create_user(task, binprm->entry);

    return task;
}
