#pragma once

#include <interrupt.h>

#define HZ 100

class Timer
{
  public:
    static void init(Interrupt*);
    static inline Timer* instance() { return _instance; }
    void connect(interrupt_handler_t*, void*);
    void disconnect();
    void reset() const;
    static long unsigned counter();
  private:
    static Timer* _instance;
    Interrupt* _interrupt;
    unsigned _quantum;

    Timer(Interrupt*);
    ~Timer();
};
