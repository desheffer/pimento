#pragma once

#include <interrupt.h>
#include <scheduler.h>
#include <stdint.h>

class Timer
{
  public:
    static Timer* instance();
    void init(Interrupt*, Scheduler*);
    static uint64_t counter();
    static void wait(unsigned);
  private:
    static Timer* _instance;
    Scheduler* _scheduler;

    Timer();
    ~Timer();
    void handleInterrupt(process_state_t*);
    static void handleInterruptStub(void*, process_state_t*);
};
