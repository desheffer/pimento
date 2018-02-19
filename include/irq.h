#include <stdint.h>

#ifndef IRQ_H
#define IRQ_H

#define NUM_IRQS 72

namespace irq
{
    typedef void (*handler_t)(void);
    typedef void (*clearer_t)(void);

    enum irq_number_t {
        system_timer_1 = 1,
        usb_controler  = 9,
        arm_timer      = 64,
    };

    void init();
    void register_handler(irq_number_t, handler_t, clearer_t);
    extern "C" void irq_handler();
}

#endif
