#include <kstdio.h>
#include <messages.h>
#include <scheduler.h>

/**
 * Finish kernel initialization after all architecture-specific steps have been
 * completed.
 */
void kernel_main(void)
{
    kputs(WELCOME_MSG);

    schedule();

    while (1);
}
