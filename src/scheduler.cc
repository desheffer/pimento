#include <scheduler.h>
#include <stdlib.h>
#include <string.h>

Scheduler* Scheduler::_instance = 0;
process_state_t* process_state = 0;

Scheduler::Scheduler()
{
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

void Scheduler::init(Timer* timer)
{
    _timer = timer;
    _nextPid = 1;
    _currentProcess = 0;

    for (unsigned i = 0; i < NUM_PROC; i++) {
        _processes[i].pid = 0;
    }

    _processes[0].pid = _nextPid++;
    memcpy(_processes[0].pname, "Init", 5);

    _timer->setTickHandler(handleTickStub, this);
}

void Scheduler::schedule()
{
    _processes[_currentProcess].state = process_state;

    // Schedule the next process using round-robin.
    do {
        _currentProcess = (_currentProcess + 1) % NUM_PROC;
    } while (!_processes[_currentProcess].pid);

    process_state = _processes[_currentProcess].state;
}

void Scheduler::handleTickStub(void* ptr)
{
    Scheduler* scheduler = (Scheduler*) ptr;
    scheduler->schedule();
}

#include <stdio.h>
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
    _processes[1].pid = _nextPid++;
    memcpy(_processes[1].pname, "myproc2", 8);

    process_state_t* state = ((process_state_t*) 0x70000) - 1; // @TODO: alloc
    memset(state, 0, sizeof(process_state_t));

    state->spsr = 0x304;
    state->elr = (uint64_t) &myproc2;
    _processes[1].state = state;
}
