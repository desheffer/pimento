#include <assert.h>
#include <list.h>
#include <limits.h>
#include <memory.h>
#include <panic.h>
#include <process.h>
#include <scheduler.h>
#include <stdlib.h>
#include <synchronize.h>
#include <timer.h>

static process_t* _current_process = 0;
static unsigned _next_pid = 1;
static list_t* _process_list = 0;
static list_t* _process_queue = 0;
static short _scheduling_queued = 0;

static void scheduler_create_init()
{
    _current_process = process_create("init", 0, 0);
    _current_process->state = running;

    list_pop_front(_process_queue);
}

static void scheduler_tick()
{
    _scheduling_queued = 1;
}

void scheduler_init()
{
    _process_list = list_new();
    _process_queue = list_new();

    scheduler_create_init();

    timer_connect(scheduler_tick, 0);
    timer_reset();
}

short unsigned scheduler_assign_pid()
{
    enter_critical();

    unsigned pid = _next_pid++;

    assert(pid <= USHRT_MAX);

    leave_critical();

    return pid;
}

void* scheduler_context_switch(void* sp)
{
    _current_process->sp = sp;

    if (_scheduling_queued) {
        _scheduling_queued = 0;

        if (_current_process->state == running) {
            _current_process->state = sleeping;
            list_push_back(_process_queue, _current_process);
        } else if (_current_process->state == stopping) {
            scheduler_destroy(_current_process);
            free(_current_process);
        }

        if (list_count(_process_queue) == 0) {
            halt();
        }

        _current_process = (process_t*) list_pop_front(_process_queue);
        _current_process->state = running;

        mmap_switch(_current_process);

        timer_reset();
    }

    return _current_process->sp;
}

unsigned scheduler_count()
{
    return list_count(_process_list);
}

process_t* scheduler_current()
{
    assert(_current_process);

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
    list_push_front(_process_queue, process);
}
