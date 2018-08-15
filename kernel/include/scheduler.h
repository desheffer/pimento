#pragma once

#include <list.h>
#include <process.h>
#include <timer.h>

class Scheduler
{
  public:
    static void init(Timer*);
    static inline Scheduler* instance() { return _instance; }
    void createProcess(const char*, const void*);
    unsigned processCount() const;
    process_regs_t* schedule(process_regs_t*);
    void stopProcess();
    static void tick(Scheduler*);
  private:
    static Scheduler* _instance;
    Timer* _timer;
    List<process_control_block_t*> _processList;
    List<process_control_block_t*> _processQueue;
    process_control_block_t* _currentProcess;
    unsigned _nextPid;
    bool _schedulingQueued;

    Scheduler(Timer*);
    ~Scheduler();
};
