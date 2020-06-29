#include <asm/mm.h>
#include <board/armv8_timer.h>
#include <board/bcm2837_interrupts.h>
#include <board/bcm2837_serial.h>
#include <board/raspberry_pi_3.h>
#include <page.h>
#include <scheduler.h>

#ifdef RASPBERRY_PI_3

extern char LD_PAGE_START;

/**
 * Initialize the Raspberry Pi 3 board.
 */
void board_init(void)
{
    page_init(&LD_PAGE_START, RASPBERRY_PI_3_MEMORY_END, PAGE_SIZE);
    bcm2837_interrupts_init();
    armv8_timer_init();
    scheduler_init(armv8_timer_set);
}

/**
 * Initialize the Raspberry Pi 3 modules.
 */
void modules_init(void)
{
    bcm2837_serial_init();
}

#endif
