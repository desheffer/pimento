#include <assert.h>
#include <bcm2837.h>
#include <limits.h>
#include <timer.h>

Timer* Timer::_instance = 0;

Timer::Timer(Interrupt* interrupt, Scheduler* scheduler)
{
    assert(interrupt);
    assert(scheduler);

    _scheduler = scheduler;

    *arm_timer_ctl = 0x003E0000;
    *arm_timer_lod = 10000;
    *arm_timer_rld = 10000;
    *arm_timer_cli = 0;

    interrupt->connect(arm_timer, handleInterruptStub, this);

    *arm_timer_ctl = 0x003E00A2;
    *arm_timer_cli = 0;
}

Timer::~Timer()
{
}

void Timer::init(Interrupt* interrupt, Scheduler* scheduler)
{
    assert(!_instance);

    _instance = new Timer(interrupt, scheduler);
}

long unsigned Timer::counter()
{
    unsigned high;
    unsigned low;

    // Loop in case the timer rolls over.
    do {
        high = *system_timer_chi;
        low = *system_timer_clo;
    } while (high != *system_timer_chi);

    return (long unsigned) high << 32 | low;
}

void Timer::handleInterrupt() const
{
    *arm_timer_cli = 0;

    _scheduler->queueScheduling();
}

void Timer::handleInterruptStub(void* ptr)
{
    Timer* timer = (Timer*) ptr;
    timer->handleInterrupt();
}
