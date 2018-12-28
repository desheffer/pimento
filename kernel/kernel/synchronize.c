#include <assert.h>
#include <interrupt.h>
#include <synchronize.h>

static unsigned _critical_level = 0;
static unsigned _daif = 0;

void enter_critical(void)
{
    unsigned daif;
    asm volatile ("mrs %0, daif" : "=r" (daif));

    disable_interrupts();

    if (_critical_level++ == 0) {
        _daif = daif;
    }
}

void leave_critical(void)
{
    assert(_critical_level > 0);

    if (--_critical_level == 0 && !(_daif & 0x80)) {
        enable_interrupts();
    }
}
