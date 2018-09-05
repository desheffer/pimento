#pragma once

#include <bcm2837.h>

#define AUX_ENABLES_MU (0b1 << 0) // Enable mini UART

#define AUX_MU_LCR_8BIT (0b11 << 0) // Enable 8-bit mode

#define GPFSEL1_FSEL14_MASK (~(0b111 << 12)) // Clear GPIO 14 function
#define GPFSEL1_FSEL15_MASK (~(0b111 << 15)) // Clear GPIO 15 function

#define GPFSEL1_FSEL14_F5 (0b010 << 12) // Set GPIO 14 to take function 5
#define GPFSEL1_FSEL15_F5 (0b010 << 15) // Set GPIO 15 to take function 5

#define GPPUDCLK0_PUD14 (0b1 << 14) // Enable GPIO 14
#define GPPUDCLK0_PUD15 (0b1 << 15) // Enable GPIO 15

#define AUX_MU_CNTL_RX (0b1 << 0) // Enable RX
#define AUX_MU_CNTL_TX (0b1 << 1) // Enable TX

#define AUX_MU_IIR_TX (0b1 << 0) // Clear TX
#define AUX_MU_IIR_RX (0b1 << 1) // Clear RX

#define AUX_MU_LSR_RX_READY (0b1 << 0) // Data ready
#define AUX_MU_LSR_TX_EMPTY (0b1 << 5) // Transmitter empty

void serial_init();
char serial_getc();
void serial_putc(char);
