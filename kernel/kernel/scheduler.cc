#include <assert.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

Scheduler* Scheduler::_instance = 0;

Scheduler::Scheduler()
{
    _nextPid = 1;
    _schedulingQueued = false;

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strcpy(process->pname, "Init");

    process->state = new process_state_t;

    _currentProcess = process;
    _processList.push_back(process);

    asm volatile(
        "msr spsel, 1\n\t"
        "mov sp, %0\n\t"
        "msr spsel, 0"
        :
        : "r" (_currentProcess->state + 1)
    );
}

Scheduler::~Scheduler()
{
}

void Scheduler::init()
{
    assert(!_instance);

    _instance = new Scheduler();
}

Scheduler* Scheduler::instance() {
    assert(_instance);

    return _instance;
}

void Scheduler::createProcess(const char* pname, const void* elr)
{
    enter_critical();

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strncpy(process->pname, pname, 32);
    process->pname[31] = '\0';

    process->state = new process_state_t;
    process->state->spsr = 0x300;
    process->state->elr = (uint64_t) elr;
    process->state->sp = (uint64_t) alloc_page() + PAGE_SIZE;

    _processList.push_back(process);
    _processQueue.push_back(process);

    leave_critical();
}

void Scheduler::queueScheduling()
{
    _schedulingQueued = true;
}

void Scheduler::schedule()
{
    if (_schedulingQueued) {
        _schedulingQueued = false;

        _processQueue.push_back(_currentProcess);
        _currentProcess = _processQueue.pop_front();
    }

    eret_handler(_currentProcess->state);
}
