#include "critical.h"
#include "interrupts.h"
#include "list.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

static struct task * _current_task = 0;
static struct list * _task_queue = 0;
static struct list * _all_tasks = 0;
static timer_set_t _timer_set;

/**
 * Initialize the scheduler. A task will be created for the currently running
 * code, which is assumed to be the "init" process.
 */
void scheduler_init(timer_set_t timer_set)
{
    _timer_set = timer_set;

    _task_queue = list_create();
    _all_tasks = list_create();

    _current_task = task_create_init();
    _current_task->state = running;

    list_push_back(_all_tasks, _current_task);
}

/**
 * Get a count of children belonging to the given parent.
 */
unsigned scheduler_count_children(struct task * parent)
{
    unsigned count = 0;

    critical_start();

    list_foreach(_all_tasks, struct task *, task) {
        if (task->parent == parent && task->state != stopped) {
            ++count;
        }
    }

    critical_end();

    return count;
}

/**
 * Get the currently running task.
 */
struct task * scheduler_current_task(void)
{
    return _current_task;
}

/**
 * Add a task to the scheduling queue.
 */
void scheduler_enqueue(struct task * task)
{
    critical_start();

    task->state = running;

    list_push_back(_all_tasks, task);
    list_push_back(_task_queue, task);

    critical_end();
}

/**
 * Replace a task in the scheduling queue.
 */
void scheduler_replace(struct task * old_task, struct task * new_task)
{
    critical_start();

    old_task->state = stopped;

    new_task->state = running;

    list_push_back(_all_tasks, new_task);
    list_push_back(_task_queue, new_task);

    critical_end();
}

/**
 * Trigger the scheduler to switch from the current task to a new task.
 */
void scheduler_schedule(void)
{
    interrupts_disable();

    critical_start();

    struct task * prev = _current_task;

    if (prev->state == running) {
        list_push_back(_task_queue, prev);
    }

    struct task * next = list_pop_front(_task_queue);

    if (next == 0) {
        fail("Failed to schedule next task.");
    }

    _current_task = next;

    critical_end();

    _timer_set(SCHEDULER_TIMER);

    switch_to(prev, next);
}

/**
 * Wait for a task to stop.
 */
struct task * scheduler_wait(struct task * parent, pid_t pid)
{
    struct task * zombie_task = 0;

    critical_start();

    list_foreach(_all_tasks, struct task *, task) {
        if (task->parent == parent && task->state == zombie && (pid == -1 || task->pid == pid)) {
            zombie_task = task;
            zombie_task->state = stopped;

            break;
        }
    }

    critical_end();

    return zombie_task;
}
