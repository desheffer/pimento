#pragma once

#include <interrupt.h>
#include <list.h>

struct process_control_block_t {
    unsigned pid;
    char pname[32];
    process_state_t* state;
};

class Scheduler
{
  public:
    static void init();
    static Scheduler* instance() { return _instance; }
    void schedule(process_state_t*);
    void spawn(); // temporary
  private:
    static Scheduler* _instance;
    List<process_control_block_t*> _processList;
    List<process_control_block_t*> _processQueue;
    process_control_block_t* _currentProcess;
    unsigned _nextPid;

    Scheduler();
    ~Scheduler();
};
