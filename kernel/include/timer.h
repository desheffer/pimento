#pragma once

#include <interrupt.h>
#include <scheduler.h>
#include <stdint.h>

class Timer
{
  public:
    static void init(Interrupt*, Scheduler*);
    static Timer* instance() { return _instance; }
    static uint64_t counter();
    static void wait(unsigned);
  private:
    static Timer* _instance;
    Scheduler* _scheduler;

    Timer(Interrupt*, Scheduler*);
    ~Timer();
    void handleInterrupt(process_state_t*);
    static void handleInterruptStub(void*, process_state_t*);
};
