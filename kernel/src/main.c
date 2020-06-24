#include <exec.h>
#include <initrd.h>
#include <pimento.h>
#include <scheduler.h>
#include <vfs.h>

/**
 * Execute the init program.
 */
static void _exec_init(void)
{
    const char * const argv[] = {"/bin/sh", 0};
    const char * const envp[] = {"HOME=/", 0};

    exec("/bin/hello", (char * const *) argv, (char * const *) envp);

    kputs("Failed to start init.\n");
    while (1);
}

/**
 * Finish kernel initialization after all architecture-specific steps have been
 * completed.
 */
void kernel_main(void)
{
    kputs(WELCOME_MSG);

    schedule();

    vfs_init();

    initrd_init();

    _exec_init();
}
