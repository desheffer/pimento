#include "asm/board.h"
#include "asm/start.h"
#include "pimento.h"

/**
 * Begin kernel initialization for AArch64 devices.
 */
void kernel_start(void)
{
    board_init();
}
