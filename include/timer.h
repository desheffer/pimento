#pragma once

#include <interrupt.h>
#include <scheduler.h>
#include <stdint.h>

struct counter_t {
    uint32_t low;
    uint32_t high;
};

class Timer
{
  public:
    static Timer* instance();
    void init(Interrupt*, Scheduler*);
    static counter_t counter();
    static void wait(unsigned);
  private:
    static Timer* _instance;
    Scheduler* _scheduler;

    Timer();
    ~Timer();
    void handleInterrupt(process_state_t*);
    static void handleInterruptStub(void*, process_state_t*);
};
