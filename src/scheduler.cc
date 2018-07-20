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

#include <timer.h>

void myproc2()
{
    while (true) {
        // Write to fd 1, ".", length 1.
        asm volatile(
            "mov x8, #64\n\t"
            "mov x0, #1\n\t"
            "mov x1, %0\n\t"
            "mov x2, %1\n\t"
            "svc #0"
            :
            : "r" ("."), "r" (1)
            : "x8", "x0", "x1", "x2"
        );
        Timer::wait(100);
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
