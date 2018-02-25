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

void Timer::init(Interrupt* interrupt)
{
    *arm_timer_ctl = 0x003E0000;
    *arm_timer_lod = 100000;
    *arm_timer_rld = 100000;
    *arm_timer_cli = 0;

    interrupt->connect(arm_timer, handleInterruptStub, this);

    *arm_timer_ctl = 0x003E00A2;
    *arm_timer_cli = 0;
}

void Timer::setTickHandler(timer_handler_t* handler, void* handlerData)
{
    _handler = handler;
    _handlerData = handlerData;
}

counter_t Timer::counter()
{
    counter_t counter;

    do {
        counter.high = *system_timer_chi;
        counter.low = *system_timer_clo;
    } while (counter.high != *system_timer_chi);

    return counter;
}

void Timer::wait(unsigned msecs)
{
    unsigned end = *system_timer_clo + msecs * 1000;

    while (*system_timer_clo < end);
}

void Timer::handleInterrupt()
{
    *arm_timer_cli = 0;

    if (_handler) {
        _handler(_handlerData);
    }
}

void Timer::handleInterruptStub(void* ptr)
{
    Timer* timer = (Timer*) ptr;
    timer->handleInterrupt();
}
