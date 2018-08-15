#include <assert.h>
#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

Scheduler* Scheduler::_instance = 0;

Scheduler::Scheduler(Timer* timer)
{
    assert(timer);

    _timer = timer;

    _nextPid = 1;
    _schedulingQueued = false;

    createProcess("Init", 0);
    _currentProcess = _processQueue.pop_front();
    _currentProcess->state = running;
}

Scheduler::~Scheduler()
{
}

void Scheduler::init(Timer* timer)
{
    assert(!_instance);

    _instance = new Scheduler(timer);

    timer->connect((interrupt_handler_t*) tick, _instance);
    tick(_instance);
}

void Scheduler::createProcess(const char* pname, const void* lr)
{
    enter_critical();

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    process->state = created;
    strncpy(process->pname, pname, 32);
    process->pname[31] = '\0';

    process->stackBegin = Memory::instance()->allocPage();
    process->stackEnd = (char*) process->stackBegin + PAGE_SIZE;

    // @TODO: Allow stack to extend beyond one page.
    process->regs = (process_regs_t*) process->stackEnd - 1;
    process->regs->spsr = 0x300;
    process->regs->lr = (long unsigned) lr;

    _processList.push_back(process);
    _processQueue.push_back(process);

    leave_critical();
}

unsigned Scheduler::processCount() const
{
    return _processList.size();
}

process_regs_t* Scheduler::schedule(process_regs_t* regs)
{
    _currentProcess->regs = regs;

    if (_schedulingQueued) {
        _schedulingQueued = false;

        if (_currentProcess->state == running) {
            _currentProcess->state = sleeping;
            _processQueue.push_back(_currentProcess);
        } else if (_currentProcess->state == stopping) {
            _processList.remove(_currentProcess);
            delete _currentProcess;
        }

        _currentProcess = _processQueue.pop_front();
    }

    _currentProcess->state = running;

    return _currentProcess->regs;
}

void Scheduler::stopProcess()
{
    enter_critical();

    _currentProcess->state = stopping;

    leave_critical();
}

void Scheduler::tick(Scheduler* scheduler)
{
    scheduler->_schedulingQueued = true;
    scheduler->_timer->reset();
}
