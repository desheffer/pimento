#include <exec.h>
#include <kstdio.h>
#include <messages.h>
#include <scheduler.h>

/**
 * Execute the init program.
 */
static void exec_init(void)
{
    const char * const argv[] = {"/bin/sh", 0};
    const char * const envp[] = {"HOME=/", 0};

    exec("/bin/sh", (char * const *) argv, (char * const *) envp);
}

/**
 * Finish kernel initialization after all architecture-specific steps have been
 * completed.
 */
void kernel_main(void)
{
    kputs(WELCOME_MSG);

    schedule();

    exec_init();
}
