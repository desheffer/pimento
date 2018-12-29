#include <assert.h>
#include <list.h>
#include <limits.h>
#include <memory.h>
#include <mm.h>
#include <panic.h>
#include <scheduler.h>
#include <synchronize.h>
#include <timer.h>

static process_t* _current_process = 0;
static unsigned _next_pid = 1;
static list_t* _process_list = 0;
static list_t* _process_queue = 0;

static void scheduler_timer(void* data)
{
    (void) data;

    scheduler_context_switch();
}

void scheduler_init(void)
{
    _process_list = list_new();
    _process_queue = list_new();

    _current_process = process_create_kernel();
    list_push_back(_process_list, _current_process);

    timer_connect(scheduler_timer, 0);
}

short unsigned scheduler_assign_pid(void)
{
    enter_critical();

    unsigned pid = _next_pid++;

    failif(pid > USHRT_MAX);

    leave_critical();

    return pid;
}

static void switch_to(process_t* prev, process_t* next)
{
    if (prev == next) {
        return;
    }

    _current_process = next;

    mm_switch_to(next);

    cpu_switch_to(prev->cpu_context, next->cpu_context);
}

void scheduler_context_switch(void)
{
    disable_interrupts();

    process_t* prev = scheduler_current();

    if (prev->state == running) {
        prev->state = sleeping;
        list_push_back(_process_queue, prev);
    }

    failif(_current_process->state == stopping);
    /* if (_current_process->state == stopping) { */
    /*     scheduler_destroy(_current_process); */
    /*     kfree(_current_process); */
    /* } */

    if (list_count(_process_queue) == 0) {
        halt();
    }

    process_t* next = (process_t*) list_pop_front(_process_queue);
    next->state = running;

    switch_to(prev, next);

    timer_reset();

    enable_interrupts();
}

unsigned scheduler_count(void)
{
    return list_count(_process_list);
}

process_t* scheduler_current(void)
{
    assert(_current_process != 0);

    return _current_process;
}

void scheduler_destroy(process_t* process)
{
    enter_critical();

    assert(process->state == stopping);

    list_remove(_process_list, process);
    process_destroy(process);

    leave_critical();
}

void scheduler_enqueue(process_t* process)
{
    list_push_back(_process_list, process);
    list_push_back(_process_queue, process);
}

void scheduler_tail(void)
{
    timer_reset();

    enable_interrupts();
}
