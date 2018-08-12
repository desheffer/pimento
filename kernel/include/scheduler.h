#pragma once

#include <list.h>
#include <process.h>

class Scheduler
{
  public:
    static void init();
    static inline Scheduler* instance() { return _instance; }
    void createProcess(const char*, const void*);
    void stopProcess();
    void queueScheduling();
    process_regs_t* schedule(process_regs_t*);
    unsigned processCount() const;
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
