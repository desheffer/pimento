#include <critical.h>
#include <kstdlib.h>
#include <kstring.h>
#include <task.h>

static unsigned _next_pid = 1;

/**
 * Create a task.
 */
static struct task * _task_create_common(const char * name)
{
    // Create a new task control block.
    struct task * task = kcalloc(sizeof(struct task));

    // Assign basic information.
    task->state = new;
    strncpy(task->name, name, sizeof(task->name));

    critical_start();

    task->pid = _next_pid++;

    critical_end();

    return task;
}

/**
 * Create a task representing the "init" process.
 */
struct task * task_create_init(void)
{
    struct task * task = _task_create_common("init");

    cpu_context_create_init(task);

    task->state = running;

    return task;
}

/**
 * Create a task to execute the given function.
 */
struct task * task_create(const char * name, task_function_t fn, void * data)
{
    struct task * task = _task_create_common(name);

    cpu_context_create(task, fn, data);

    return task;
}
