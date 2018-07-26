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

    createProcess("Init", 0);
    _currentProcess = _processQueue.pop_front();
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

void Scheduler::createProcess(const char* pname, const void* lr)
{
    enter_critical();

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strncpy(process->pname, pname, 32);
    process->pname[31] = '\0';

    process->stackBegin = alloc_page();
    process->stackEnd = (char*) process->stackBegin + PAGE_SIZE;

    // @TODO: Allow stack to extend beyond one page.
    process->state = (process_state_t*) process->stackEnd - 1;
    process->state->spsr = 0x300;
    process->state->lr = (uint64_t) lr;

    _processList.push_back(process);
    _processQueue.push_back(process);

    leave_critical();
}

void Scheduler::queueScheduling()
{
    _schedulingQueued = true;
}

process_state_t* Scheduler::schedule(process_state_t* state)
{
    _currentProcess->state = state;

    if (_schedulingQueued) {
        _schedulingQueued = false;

        _processQueue.push_back(_currentProcess);
        _currentProcess = _processQueue.pop_front();
    }

    return _currentProcess->state;
}
