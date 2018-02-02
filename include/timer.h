#include <stdint.h>

#ifndef TIMER_H
#define TIMER_H

namespace timer {
    static inline void wait(int32_t count)
    {
        count *= 1000;

        asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                : "=r"(count): [count]"0"(count) : "cc");
    }

    static inline void nop()
    {
        asm volatile("nop");
    }
}

#endif
