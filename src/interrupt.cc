#include <assert.h>
#include <interrupt.h>
#include <mmio.h>

Interrupt* Interrupt::_instance = 0;

Interrupt::Interrupt()
{
    for (unsigned i = 0; i < NUM_IRQS; i++) {
        _handlers[i] = 0;
        _handlerData[i] = 0;
    }
}

Interrupt::~Interrupt()
{
}

Interrupt* Interrupt::instance()
{
    if (!_instance) {
        _instance = new Interrupt();
    }
    return _instance;
}

void Interrupt::init()
{
    *irq_disable_gpu1 = 0xFFFFFFFF;
    *irq_disable_gpu2 = 0xFFFFFFFF;
    *irq_disable_basic = 0xFFFFFFFF;

    enable_interrupts();
}

bool Interrupt::isPending(unsigned num)
{
    assert(num < NUM_IRQS);

    if (num < 32) {
        return *irq_pending_gpu1 & (1 << (num - 0));
    } else if (num < 64) {
        return *irq_pending_gpu2 & (1 << (num - 32));
    } else {
        return *irq_pending_basic & (1 << (num - 64));
    }
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

void Interrupt::enable(irq_number_t num)
{
    assert(num < NUM_IRQS);

    if (num < 32) {
        *irq_enable_gpu1 |= (1 << (num - 0));
    } else if (num < 64) {
        *irq_enable_gpu2 |= (1 << (num - 32));
    } else {
        *irq_enable_basic |= (1 << (num - 64));
    }
}

void Interrupt::disable(irq_number_t num)
{
    assert(num < NUM_IRQS);

    if (num < 32) {
        *irq_enable_gpu1 ^= (1 << (num - 0));
    } else if (num < 64) {
        *irq_enable_gpu2 ^= (1 << (num - 32));
    } else {
        *irq_enable_basic ^= (1 << (num - 64));
    }
}

void Interrupt::handle(process_state_t* state)
{
    for (unsigned num = 0; num < NUM_IRQS; num++) {
        if (instance()->isPending(num) && _handlers[num]) {
            _handlers[num](_handlerData[num], state);
            break;
        }
    }
}

void enable_interrupts()
{
    asm volatile("msr daifclr, #2");
}

void disable_interrupts()
{
    asm volatile("msr daifset, #2");
}

void irq_handler(process_state_t* state)
{
    Interrupt::instance()->handle(state);
}
