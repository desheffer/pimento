#include <scheduler.h>
#include <system.h>

void do_exit(int status)
{
    (void) status;

    scheduler_stop_process();

    while (1) {
        asm volatile("wfi");
    }
}
