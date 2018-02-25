#include <interrupt.h>
#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

struct counter_t {
    uint32_t low;
    uint32_t high;
};

typedef void timer_handler_t(void*);

class Timer
{
  public:
    static Timer* instance();
    void init(Interrupt*);
    void setTickHandler(timer_handler_t*, void*);
    static counter_t counter();
    static void wait(unsigned);
  private:
    static Timer* _instance;
    timer_handler_t* _handler;
    void* _handlerData;

    Timer();
    ~Timer();
    void handleInterrupt();
    static void handleInterruptStub(void*);
};

#endif
