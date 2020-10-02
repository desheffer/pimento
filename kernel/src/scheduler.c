#include "critical.h"
#include "interrupts.h"
#include "list.h"
#include "pimento.h"
#include "scheduler.h"
#include "task.h"

static struct task * _current_task = 0;
static struct list * _task_queue = 0;
static timer_set_t _timer_set;

/**
 * Initialize the scheduler. A task will be created for the currently running
 * code, which is assumed to be the "init" process.
 */
void scheduler_init(timer_set_t timer_set)
{
    _timer_set = timer_set;

    _task_queue = list_create();

    _current_task = task_create_init();
    _current_task->state = running;
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

    list_push_back(_task_queue, new_task);

    critical_end();
}

/**
 * Trigger the scheduler to switch from the current task to a new task.
 */
void schedule(void)
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
