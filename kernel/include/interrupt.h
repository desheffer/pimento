#pragma once

#include <stdint.h>

#define NUM_IRQS 72

enum irq_number_t {
    system_timer_1 = 1,
    usb_controler  = 9,
    arm_timer      = 64,
};

typedef void interrupt_handler_t(void*);

class Interrupt
{
  public:
    static void init();
    static Interrupt* instance();
    bool isPending(unsigned) const;
    void connect(irq_number_t, interrupt_handler_t*, void*);
    void disconnect(irq_number_t);
    void enable(irq_number_t) const;
    void disable(irq_number_t) const;
    void handle() const;

  private:
    static Interrupt* _instance;
    interrupt_handler_t* _handlers[NUM_IRQS];
    void* _handlerData[NUM_IRQS];

    Interrupt();
    ~Interrupt();
};

extern "C" void enable_interrupts();
extern "C" void disable_interrupts();
