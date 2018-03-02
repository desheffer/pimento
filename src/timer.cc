#include <assert.h>
#include <limits.h>
#include <mmio.h>
#include <timer.h>

Timer* Timer::_instance = 0;

Timer::Timer()
{
}

Timer::~Timer()
{
}

Timer* Timer::instance()
{
    if (!_instance) {
        _instance = new Timer();
    }
    return _instance;
}

void Timer::init(Interrupt* interrupt, Scheduler* scheduler)
{
    _scheduler = scheduler;

    *arm_timer_ctl = 0x003E0000;
    *arm_timer_lod = 10000;
    *arm_timer_rld = 10000;
    *arm_timer_cli = 0;

    interrupt->connect(arm_timer, handleInterruptStub, this);

    *arm_timer_ctl = 0x003E00A2;
    *arm_timer_cli = 0;
}

uint64_t Timer::counter()
{
    uint64_t high;
    uint64_t low;

    // Loop in case the timer rolls over.
    do {
        high = *system_timer_chi;
        low = *system_timer_clo;
    } while (high != *system_timer_chi);

    return (high << 32) | low;
}

void Timer::wait(unsigned msecs)
{
    uint64_t end = counter() + ((uint64_t) msecs) * 1000;

    while (counter() < end);
}

void Timer::handleInterrupt(process_state_t* state)
{
    *arm_timer_cli = 0;

    assert(_scheduler);
    _scheduler->schedule(state);
}

void Timer::handleInterruptStub(void* ptr, process_state_t* state)
{
    Timer* timer = (Timer*) ptr;
    timer->handleInterrupt(state);
}
