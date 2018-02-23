#ifndef INTERRUPT_H
#define INTERRUPT_H

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
    static Interrupt* instance();
    void init();
    bool isPending(unsigned);
    void connect(irq_number_t, interrupt_handler_t*, void*);
    void disconnect(irq_number_t);
    void enable(irq_number_t);
    void disable(irq_number_t);
    void handle();

  private:
    static Interrupt* _instance;
    interrupt_handler_t* _handlers[NUM_IRQS];
    void* _handlerData[NUM_IRQS];

    Interrupt();
    ~Interrupt();
    static void enableInterrupts();
    static void disableInterrupts();
};

#endif
