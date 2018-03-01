#include <scheduler.h>
#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <timer.h>

Scheduler* Scheduler::_instance = 0;

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

void Scheduler::init()
{
    _nextPid = 1;
    _currentProcess = 0;

    for (unsigned i = 0; i < NUM_PROC; i++) {
        _processes[i].pid = 0;
    }

    _processes[0].pid = _nextPid++;
    memcpy(_processes[0].pname, "Init", 5);
    _processes[0].state = new process_state_t;
}

void Scheduler::schedule(process_state_t* state)
{
    memcpy(_processes[_currentProcess].state, state, sizeof(process_state_t));

    // Schedule the next process using round-robin.
    do {
        _currentProcess = (_currentProcess + 1) % NUM_PROC;
    } while (!_processes[_currentProcess].pid);

    memcpy(state, _processes[_currentProcess].state, sizeof(process_state_t));
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
    _processes[1].pid = _nextPid++;
    memcpy(_processes[1].pname, "myproc2", 8);

    _processes[1].state = new process_state_t;
    _processes[1].state->spsr = 0x304;
    _processes[1].state->elr = (uint64_t) &myproc2;
    _processes[1].state->sp = (uint64_t) ((char*) malloc(4096) + 4096);
}
