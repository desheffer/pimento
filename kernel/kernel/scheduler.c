#include <assert.h>
#include <list.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>
#include <timer.h>

static list_t* _process_list;
static list_t* _process_queue;
static process_control_block_t* _current_process;
static unsigned _next_pid;
static short _scheduling_queued;

void scheduler_init()
{
    _process_list = malloc(sizeof(list_t));
    _process_list->front = 0;
    _process_list->back = 0;

    _process_queue = malloc(sizeof(list_t));
    _process_queue->front = 0;
    _process_queue->back = 0;

    _next_pid = 1;
    _scheduling_queued = 0;

    scheduler_create_process("Init", 0);
    _current_process = (process_control_block_t*) pop_front(_process_queue);
    _current_process->state = running;

    timer_connect(scheduler_tick, 0);
    scheduler_tick();
}

void scheduler_create_process(const char* pname, const void* lr)
{
    enter_critical();

    process_control_block_t* process = malloc(sizeof(process_control_block_t));
    process->pid = _next_pid++;
    process->state = created;
    strncpy(process->pname, pname, 32);
    process->pname[31] = '\0';

    process->stack_begin = alloc_page();
    process->stack_end = (char*) process->stack_begin + PAGE_SIZE;

    // @TODO: Allow stack to extend beyond one page.
    process->regs = (process_regs_t*) process->stack_end - 1;
    process->regs->spsr = 0x300;
    process->regs->lr = (long unsigned) lr;

    push_back(_process_list, process);
    push_back(_process_queue, process);

    leave_critical();
}

unsigned scheduler_process_count()
{
    return count(_process_list);
}

process_regs_t* scheduler_schedule(process_regs_t* regs)
{
    _current_process->regs = regs;

    if (_scheduling_queued) {
        _scheduling_queued = 0;

        if (_current_process->state == running) {
            _current_process->state = sleeping;
            push_back(_process_queue, _current_process);
        } else if (_current_process->state == stopping) {
            remove(_process_list, _current_process);
            free(_current_process);
        }

        _current_process = (process_control_block_t*) pop_front(_process_queue);
    }

    _current_process->state = running;

    return _current_process->regs;
}

void scheduler_stop_process()
{
    enter_critical();

    _current_process->state = stopping;

    leave_critical();
}

void scheduler_tick()
{
    _scheduling_queued = 1;

    timer_reset();
}
