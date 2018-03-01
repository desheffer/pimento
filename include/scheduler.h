#pragma once

#include <interrupt.h>

#define NUM_PROC 64

struct process_control_block_t {
    unsigned pid;
    char pname[32];
    process_state_t* state;
};

class Scheduler
{
  public:
    static Scheduler* instance();
    void init();
    void schedule(process_state_t*);
    void spawn(); // temporary
  private:
    static Scheduler* _instance;
    process_control_block_t _processes[NUM_PROC]; // @TODO: use non-fixed length
    unsigned _currentProcess;
    unsigned _nextPid;

    Scheduler();
    ~Scheduler();
};
