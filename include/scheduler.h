#pragma once

#include <timer.h>

#define NUM_REGS 31
#define NUM_PROC 64

struct process_state_t {
    volatile uint64_t spsr;
    volatile uint64_t elr;
    volatile uint64_t xzr; // Padding
    volatile uint64_t x[NUM_REGS];
};

struct process_control_block_t {
    unsigned pid;
    char pname[32];
    process_state_t* state;
};

extern "C" process_state_t* process_state;

class Scheduler
{
  public:
    static Scheduler* instance();
    void init(Timer*);
    void spawn();
  private:
    static Scheduler* _instance;
    Timer* _timer;
    process_control_block_t _processes[NUM_PROC]; // @TODO: use non-fixed length
    unsigned _currentProcess;
    unsigned _nextPid;

    Scheduler();
    ~Scheduler();
    void schedule();
    static void handleTickStub(void*);
};
