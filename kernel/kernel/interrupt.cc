#include <assert.h>
#include <bcm2837.h>
#include <interrupt.h>
#include <scheduler.h>

Interrupt* Interrupt::_instance = 0;

Interrupt::Interrupt()
{
    for (unsigned i = 0; i < NUM_IRQS; ++i) {
        _handlers[i] = 0;
        _handlerData[i] = 0;
    }
}

Interrupt::~Interrupt()
{
}

void Interrupt::init()
{
    assert(!_instance);

    _instance = new Interrupt();

    enable_interrupts();
}

bool Interrupt::isPending(unsigned num) const
{
    assert(num < NUM_IRQS);

    return *core0_irq & (1 << num);
}

void Interrupt::connect(irq_number_t num, interrupt_handler_t* handler, void* handlerData)
{
    assert(num < NUM_IRQS);

    _handlers[num] = handler;
    _handlerData[num] = handlerData;

    enable(num);
}

void Interrupt::disconnect(irq_number_t num)
{
    assert(num < NUM_IRQS);

    disable(num);

    _handlers[num] = 0;
    _handlerData[num] = 0;
}

void Interrupt::enable(irq_number_t num) const
{
    assert(num < NUM_IRQS);

    *core0_timers_cntl |= (1 << num);
}

void Interrupt::disable(irq_number_t num) const
{
    assert(num < NUM_IRQS);

    *core0_timers_cntl ^= (1 << num);
}

void Interrupt::handle() const
{
    for (unsigned num = 0; num < NUM_IRQS; ++num) {
        if (instance()->isPending(num) && _handlers[num]) {
            _handlers[num](_handlerData[num]);
            break;
        }
    }
}

process_regs_t* irq_handler(process_regs_t* state)
{
    Interrupt::instance()->handle();

    return Scheduler::instance()->schedule(state);
}
