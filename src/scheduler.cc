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

    auto process = new process_control_block_t;
    process->pid = _nextPid++;
    strcpy(process->pname, "Init");

    process->state = new process_state_t;

    _currentProcess = process;
    _processList.push_back(process);
}

Scheduler::~Scheduler()
{
}

void Scheduler::init()
{
    assert(!_instance);

    _instance = new Scheduler();
}

void Scheduler::schedule(process_state_t* state)
{
    memcpy(_currentProcess->state, state, sizeof(process_state_t));

    _processQueue.push_back(_currentProcess);
    _currentProcess = _processQueue.pop_front();

    memcpy(state, _currentProcess->state, sizeof(process_state_t));
}

#include <stdio.h>
#include <timer.h>

void myproc2()
{
    Timer::wait(300);

    while (true) {
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
