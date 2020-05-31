#include <asm/head.h>
#include <critical.h>
#include <interrupts.h>

static unsigned _critical_level = 0;
static unsigned _daif = 0;

/**
 * Enter a critical (uninterruptable) section of code.
 */
void critical_start(void)
{
    unsigned daif;
    asm volatile ("mrs %0, daif" : "=r" (daif));

    interrupts_disable();

    if (_critical_level == 0) {
        _daif = daif;
    }

    ++_critical_level;
}

/**
 * Leave a critical section of code.
 */
void critical_end(void)
{
    --_critical_level;

    if (_critical_level == 0 && !(_daif & PSR_MODE_I)) {
        interrupts_enable();
    }
}
