#include <irq.h>
#include <mmio.h>
#include <stdint.h>
#include <timer.h>

#include <stdio.h>

namespace timer
{
    static void irq_handler()
    {
        printf("t");
    }

    static void irq_clearer()
    {
        *mmio::arm_timer_cli = 0;
    }

    void init()
    {
        *mmio::arm_timer_ctl = 0x003E0000;
        *mmio::arm_timer_lod = 2000000-1;
        *mmio::arm_timer_rld = 2000000-1;
        *mmio::arm_timer_cli = 0;

        irq::register_handler(irq::arm_timer, irq_handler, irq_clearer);

        *mmio::arm_timer_ctl = 0x003E00A2;
        *mmio::arm_timer_cli = 0;
    }

    counter_t counter()
    {
        counter_t counter;

        do {
            counter.high = *mmio::system_timer_chi;
            counter.low = *mmio::system_timer_clo;
        } while (counter.high != *mmio::system_timer_chi);

        return counter;
    }

    void wait(uint32_t msecs)
    {
        uint32_t end = *mmio::system_timer_clo + msecs * 1000;

        while (*mmio::system_timer_clo < end);
    }
}
