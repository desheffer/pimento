#include <interrupt.h>
#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

struct counter_t {
    uint32_t low;
    uint32_t high;
};

class Timer
{
  public:
    static Timer* instance();
    void init(Interrupt*);
    static counter_t counter();
    static void wait(unsigned);
  private:
    static Timer* _instance;

    Timer();
    ~Timer();
    void handleInterrupt();
    static void handleInterruptStub(void*);
};

#endif
