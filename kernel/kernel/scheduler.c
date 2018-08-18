#include <assert.h>
#include <list.h>
#include <scheduler.h>
#include <stdlib.h>
#include <synchronize.h>
#include <timer.h>

static process_control_block_t* _current_process = 0;
static list_t* _process_queue = 0;
static short _scheduling_queued = 0;

void scheduler_init()
{
    _process_queue = malloc(sizeof(list_t));
    _process_queue->front = 0;
    _process_queue->back = 0;
}

void scheduler_add(process_control_block_t* process)
{
    enter_critical();

    push_front(_process_queue, process);

    leave_critical();
}

unsigned scheduler_current_pid()
{
    assert(_current_process);

    return _current_process->pid;
}

process_regs_t* scheduler_next(process_regs_t* regs)
{
    _current_process->regs = regs;

    if (_scheduling_queued) {
        _scheduling_queued = 0;

        if (_current_process->state == running) {
            _current_process->state = sleeping;
            push_back(_process_queue, _current_process);
        } else if (_current_process->state == stopping) {
            process_destroy(_current_process->pid);
            free(_current_process);
        }

        timer_reset();

        _current_process = (process_control_block_t*) pop_front(_process_queue);
    }

    _current_process->state = running;

    return _current_process->regs;
}

void scheduler_start()
{
    assert(_process_queue->front);
    assert(_current_process == 0);

    _current_process = (process_control_block_t*) pop_front(_process_queue);
    _current_process->state = running;

    timer_connect(scheduler_tick, 0);
    timer_reset();
}

void scheduler_tick()
{
    _scheduling_queued = 1;
}
