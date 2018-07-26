#pragma once

#include <list.h>
#include <process.h>

class Scheduler
{
  public:
    static void init();
    static Scheduler* instance();
    void createProcess(const char*, const void*);
    void queueScheduling();
    process_state_t* schedule(process_state_t*);
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
