#pragma once

#include <process.h>

#define NUM_IRQS 10

enum irq_number_t {
    local_irq_cntpsirq  = 0,
    local_irq_cntpnsirq = 1,
    local_irq_cnthpirq  = 2,
    local_irq_cntvirq   = 3,
    local_irq_mailbox0  = 4,
    local_irq_mailbox1  = 5,
    local_irq_mailbox2  = 6,
    local_irq_mailbox3  = 7,
    local_irq_gpu_fast  = 8,
    local_irq_pmu_fast  = 9,
};

typedef void interrupt_handler_t(void*);

class Interrupt
{
  public:
    static void init();
    static inline Interrupt* instance() { return _instance; }
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
extern "C" process_regs_t* irq_handler(process_regs_t* state);
