#include <assert.h>
#include <fork.h>
#include <list.h>
#include <memory.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <timer.h>

static process_t* _current_process = 0;
static unsigned _next_pid = 1;
static list_t* _process_list = 0;
static list_t* _process_queue = 0;
static short _scheduling_queued = 0;

static void process_create_init()
{
    _current_process = process_create("init", 0, 0);
    _current_process->state = running;

    pop_front(_process_queue);
}

static void process_tick()
{
    _scheduling_queued = 1;
}

void process_init()
{
    _process_list = malloc(sizeof(list_t));
    _process_list->front = 0;
    _process_list->back = 0;

    _process_queue = malloc(sizeof(list_t));
    _process_queue->front = 0;
    _process_queue->back = 0;

    process_create_init();

    timer_connect(process_tick, 0);
    timer_reset();
}

unsigned process_assign_pid()
{
    enter_critical();

    unsigned pid = _next_pid++;

    leave_critical();

    return pid;
}

process_regs_t* process_context_switch(process_regs_t* regs)
{
    _current_process->regs = regs;

    if (_scheduling_queued) {
        _scheduling_queued = 0;

        if (_current_process->state == running) {
            _current_process->state = sleeping;
            push_back(_process_queue, _current_process);
        } else if (_current_process->state == stopping) {
            process_destroy(_current_process);
            free(_current_process);
        }

        _current_process = (process_t*) pop_front(_process_queue);
        _current_process->state = running;

        mmap_switch(_current_process);

        timer_reset();
    }

    return _current_process->regs;
}

unsigned process_count()
{
    return count(_process_list);
}

process_t* process_current()
{
    assert(_current_process);

    return _current_process;
}

void process_destroy(process_t* process)
{
    enter_critical();

    assert(process->state == stopping);

    // @TODO: Call free_page() for each entry in process->pages.

    remove(_process_list, process);

    leave_critical();
}

void process_enqueue(process_t* process)
{
    push_back(_process_list, process);
    push_front(_process_queue, process);
}
