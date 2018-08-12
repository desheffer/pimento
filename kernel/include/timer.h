#pragma once

#include <interrupt.h>
#include <scheduler.h>

class Timer
{
  public:
    static void init(Interrupt*, Scheduler*);
    static inline Timer* instance() { return _instance; }
    static long unsigned counter();
  private:
    static Timer* _instance;
    Scheduler* _scheduler;

    Timer(Interrupt*, Scheduler*);
    ~Timer();
    void handleInterrupt() const;
    static void handleInterruptStub(void*);
};
