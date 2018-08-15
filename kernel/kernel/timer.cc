#include <assert.h>
#include <bcm2837.h>
#include <limits.h>
#include <timer.h>

Timer* Timer::_instance = 0;

Timer::Timer(Interrupt* interrupt)
{
    assert(interrupt);

    _interrupt = interrupt;

    unsigned freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));
    _quantum = freq / HZ;

    // Use Crystal clock and increment by 1.
    *core_timers_control = 0;

    // Set timer prescaler 1:1 (timer freq = 2^31 / prescaler * input).
    *core_timers_prescaler = 0x80000000;
}

Timer::~Timer()
{
}

void Timer::init(Interrupt* interrupt)
{
    assert(!_instance);

    _instance = new Timer(interrupt);
}

void Timer::connect(interrupt_handler_t* handler, void* handlerData)
{
    _interrupt->connect(local_irq_cntpnsirq, handler, handlerData);
}

void Timer::disconnect()
{
    _interrupt->disconnect(local_irq_cntpnsirq);
}

void Timer::reset() const
{
    // Set down-count timer.
    asm volatile("msr cntp_tval_el0, %0" :: "r" (_quantum));

    // Enable the timer.
    asm volatile("msr cntp_ctl_el0, %0" :: "r" (1));
}

long unsigned Timer::counter()
{
    long unsigned count;

    // Get the timer value.
    asm volatile("mrs %0, cntpct_el0" : "=r" (count));

    return count;
}
