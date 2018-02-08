#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

namespace timer {
    struct counter_t {
        uint32_t low;
        uint32_t high;
    };

    inline void nop()
    {
        asm volatile("nop");
    }

    counter_t counter();
    void wait(uint32_t);
}

#endif
