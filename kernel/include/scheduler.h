#pragma once

#include <interrupt.h>
#include <list.h>

#define NUM_REGS 31

struct process_state_t {
    volatile uint64_t spsr;
    volatile uint64_t elr;
    volatile uint64_t sp;
    volatile uint64_t x[NUM_REGS];
};

struct process_control_block_t {
    unsigned pid;
    char pname[32];
    process_state_t* state;
};

class Scheduler
{
  public:
    static void init();
    static Scheduler* instance();
    void queueScheduling();
    void schedule(process_state_t*);
    void spawn(); // temporary
  private:
    static Scheduler* _instance;
    List<process_control_block_t*> _processList;
    List<process_control_block_t*> _processQueue;
    process_control_block_t* _currentProcess;
    unsigned _nextPid;
    bool _schedulingQueued;

    Scheduler();
    ~Scheduler();
};

extern "C" void eret_handler(process_state_t*);
