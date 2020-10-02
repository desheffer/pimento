#include "board/armv8_timer.h"
#include "pimento.h"
#include "scheduler.h"

static unsigned _cntfrq = 0;

/**
 * Listen for ticks and delegate to the scheduler.
 */
void armv8_timer_tick(void * data)
{
    (void) data;

    schedule();
}

/**
 * Initialize the ARMv8 timer.
 */
void armv8_timer_init(void)
{
    asm volatile("mrs %0, cntfrq_el0" : "=r" (_cntfrq));
}

/**
 * Set the timer.
 */
void armv8_timer_set(unsigned ms)
{
    // Set down-count timer.
    asm volatile("msr cntp_tval_el0, %0" :: "r" (_cntfrq / 1000 * ms));

    // Enable the timer.
    asm volatile("msr cntp_ctl_el0, %0" :: "r" (1));
}
