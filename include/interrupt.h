#pragma once

#include <stdint.h>

#define NUM_REGS 31
#define NUM_IRQS 72

struct process_state_t {
    volatile uint64_t spsr;
    volatile uint64_t elr;
    volatile uint64_t sp;
    volatile uint64_t x[NUM_REGS];
};

enum irq_number_t {
    system_timer_1 = 1,
    usb_controler  = 9,
    arm_timer      = 64,
};

typedef void interrupt_handler_t(void*, process_state_t*);

class Interrupt
{
  public:
    static Interrupt* instance();
    void init();
    bool isPending(unsigned);
    void connect(irq_number_t, interrupt_handler_t*, void*);
    void disconnect(irq_number_t);
    void enable(irq_number_t);
    void disable(irq_number_t);
    void handle(process_state_t*);

  private:
    static Interrupt* _instance;
    interrupt_handler_t* _handlers[NUM_IRQS];
    void* _handlerData[NUM_IRQS];

    Interrupt();
    ~Interrupt();
    static void enableInterrupts();
    static void disableInterrupts();
};

extern "C" void irq_handler(process_state_t*);
