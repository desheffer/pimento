#include <assert.h>
#include <bcm2837.h>
#include <limits.h>
#include <timer.h>

static unsigned _quantum;

void timer_init()
{
    unsigned freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r" (freq));
    _quantum = freq / HZ;

    // Use Crystal clock and increment by 1.
    *core_timers_control = 0;

    // Set timer prescaler 1:1 (timer freq = 2^31 / prescaler * input).
    *core_timers_prescaler = 0x80000000;
}

void timer_connect(interrupt_handler_t* handler, void* handler_data)
{
    interrupt_connect(local_irq_cntpnsirq, handler, handler_data);
}

void timer_disconnect()
{
    interrupt_disconnect(local_irq_cntpnsirq);
}

void timer_reset()
{
    // Set down-count timer.
    asm volatile("msr cntp_tval_el0, %0" :: "r" (_quantum));

    // Enable the timer.
    asm volatile("msr cntp_ctl_el0, %0" :: "r" (1));
}

long unsigned timer_counter()
{
    long unsigned count;

    // Get the timer value.
    asm volatile("mrs %0, cntpct_el0" : "=r" (count));

    return count;
}
