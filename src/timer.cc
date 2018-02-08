#include <mmio.h>
#include <stdint.h>
#include <timer.h>

namespace timer
{
    counter_t counter()
    {
        counter_t counter;

        counter.high = *mmio::system_timer_chi;
        counter.low = *mmio::system_timer_clo;

        return counter;
    }

    void wait(uint32_t msecs)
    {
        uint32_t end = *mmio::system_timer_clo + msecs * 1000;

        while (*mmio::system_timer_clo < end);
    }
}
