#include "asm/entry.h"
#include "pimento.h"

static interrupts_handler_t _interrupts_handler;

/**
 * Handle interrupts. This method delegates to a device-specific method.
 */
void interrupts_handler()
{
    if (_interrupts_handler != 0) {
        _interrupts_handler();
    }
}

/**
 * Set the method used by `interrupts_handler`.
 */
void set_interrupts_handler(interrupts_handler_t new_interrupts_handler)
{
    _interrupts_handler = new_interrupts_handler;
}
