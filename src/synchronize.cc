#include <assert.h>
#include <interrupt.h>
#include <synchronize.h>

static unsigned _critical_level = 0;
static bool _reenable_interrupts;

void enter_critical()
{
    uint64_t daif;
    asm volatile ("mrs %0, daif" : "=r" (daif));

    disable_interrupts();

    if (_critical_level++ == 0) {
        _reenable_interrupts = !(daif & 0x80);
    }
}

void leave_critical()
{
    assert(_critical_level > 0);

    if (--_critical_level == 0 && _reenable_interrupts) {
        enable_interrupts();
    }
}
