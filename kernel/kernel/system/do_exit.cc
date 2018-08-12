#include <scheduler.h>
#include <system.h>

void do_exit(int /*status*/)
{
    Scheduler::instance()->stopProcess();

    while (true) {
        asm volatile("wfi");
    }
}
