#include <assert.h>
#include <list.h>
#include <limits.h>
#include <memory.h>
#include <mm.h>
#include <panic.h>
#include <scheduler.h>
#include <synchronize.h>
#include <timer.h>

static struct process * _current_process = 0;
static unsigned _next_pid = 0;
static struct list * _process_list = 0;
static struct list * _process_queue = 0;

static void scheduler_tick(void * data)
{
    (void) data;

    scheduler_context_switch();
}

void scheduler_init(void)
{
    _process_list = list_new();
    _process_queue = list_new();

    _current_process = process_create_kernel();
    _current_process->state = running;
    list_push_back(_process_list, _current_process);

    timer_connect(scheduler_tick, 0);
}

unsigned scheduler_assign_pid(void)
{
    enter_critical();

    unsigned pid = _next_pid++;
    failif(pid > USHRT_MAX);

    leave_critical();

    return pid;
}

static void switch_to(struct process * prev, struct process * next)
{
    if (prev == next) {
        return;
    }

    mm_switch_to(next);
    cpu_switch_to(prev->cpu_context, next->cpu_context);
}

void scheduler_context_switch(void)
{
    disable_interrupts();

    struct process * prev = scheduler_current();

    if (prev->state == running) {
        prev->state = sleeping;
        list_push_back(_process_queue, prev);
    } else if (prev->state == stopped) {
        scheduler_destroy(prev);
    }

    assert(list_count(_process_queue) > 0);

    struct process * next = (struct process *) list_pop_front(_process_queue);
    next->state = running;

    _current_process = next;

    switch_to(prev, next);

    scheduler_tail();
}

unsigned scheduler_count(void)
{
    enter_critical();

    unsigned count = list_count(_process_list);

    leave_critical();

    return count;
}

struct process * scheduler_current(void)
{
    enter_critical();

    assert(_current_process != 0);

    struct process * process = _current_process;

    leave_critical();

    return process;
}

void scheduler_destroy(struct process * process)
{
    enter_critical();

    assert(process->state == stopped);

    list_remove(_process_list, process);
    process_destroy(process);

    leave_critical();
}

void scheduler_enqueue(struct process * process)
{
    enter_critical();

    list_push_back(_process_list, process);
    list_push_back(_process_queue, process);

    leave_critical();
}

void scheduler_exit(struct process * process)
{
    enter_critical();

    process->state = stopped;

    leave_critical();
}

struct process * scheduler_get_pid(unsigned pid)
{
    enter_critical();

    struct list_item * process_item = _process_list->front;
    struct process * process = 0;

    while (process_item != 0) {
        process = (struct process *) process_item->item;
        if (process->pid == pid) {
            break;
        }

        process_item = process_item->next;
        process = 0;
    }

    leave_critical();

    return process;
}

void scheduler_tail(void)
{
    timer_reset();

    enable_interrupts();
}
