#include <assert.h>
#include <bcm2837.h>
#include <interrupt.h>
#include <mm.h>

static interrupt_handler_t * _handlers[NUM_IRQS] = {0};
static void * _handlers_data[NUM_IRQS] = {0};

static void interrupt_disable(enum irq_number num)
{
    assert(num < NUM_IRQS);

    *CORE0_TIMERS_CNTL ^= (0b1 << num);
}

static void interrupt_enable(enum irq_number num)
{
    assert(num < NUM_IRQS);

    *CORE0_TIMERS_CNTL |= (0b1 << num);
}

static int interrupt_pending(unsigned num)
{
    assert(num < NUM_IRQS);

    return *CORE0_IRQ & (0b1 << num);
}

void interrupt_init(void)
{
    for (unsigned i = 0; i < NUM_IRQS; ++i) {
        _handlers[i] = 0;
        _handlers_data[i] = 0;
    }

    enable_interrupts();
}

void interrupt_connect(enum irq_number num, interrupt_handler_t * handler, void * handler_data)
{
    assert(num < NUM_IRQS);

    _handlers[num] = handler;
    _handlers_data[num] = handler_data;

    interrupt_enable(num);
}

void interrupt_disconnect(enum irq_number num)
{
    assert(num < NUM_IRQS);

    interrupt_disable(num);

    _handlers[num] = 0;
    _handlers_data[num] = 0;
}

void interrupt_handler(void)
{
    for (unsigned num = 0; num < NUM_IRQS; ++num) {
        if (interrupt_pending(num) && _handlers[num]) {
            _handlers[num](_handlers_data[num]);
            break;
        }
    }
}
