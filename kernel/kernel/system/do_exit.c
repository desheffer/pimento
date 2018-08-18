#include <scheduler.h>
#include <system.h>

void do_exit(int status)
{
    (void) status;

    unsigned pid = scheduler_current_pid();
    process_stop(pid);

    while (1) {
        asm volatile("wfi");
    }
}
