#include <memory.h>
#include <scheduler.h>
#include <stdlib.h>
#include <string.h>
#include <synchronize.h>

#include <stdio.h>
#include <timer.h>

Scheduler* Scheduler::_instance = 0;

Scheduler::Scheduler()
{
    _nextPid = 1;
}

Scheduler::~Scheduler()
{
}

Scheduler* Scheduler::instance()
{
    if (!_instance) {
        _instance = new Scheduler();
    }
    return _instance;
}

void Scheduler::init()
{
    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strcpy(process->pname, "Init");

    process->state = new process_state_t;

    _currentProcess = process;
    _processList.push_back(process);
}

void Scheduler::schedule(process_state_t* state)
{
    memcpy(_currentProcess->state, state, sizeof(process_state_t));

    _processQueue.push_back(_currentProcess);
    _currentProcess = _processQueue.pop_front();

    memcpy(state, _currentProcess->state, sizeof(process_state_t));
}

void myproc2()
{
    Timer::wait(300);

    while (1) {
        printf("\n[proc 2]");

        Timer::wait(1000);
    }
}

void Scheduler::spawn()
{
    enter_critical();

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strcpy(process->pname, "myproc2");

    process->state = new process_state_t;
    process->state->spsr = 0x304;
    process->state->elr = (uint64_t) &myproc2;
    process->state->sp = (uint64_t) alloc_page() + PAGE_SIZE;

    _processList.push_back(process);
    _processQueue.push_back(process);

    leave_critical();
}
