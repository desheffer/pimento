#pragma once

#include <stdint.h>

#define GPFSEL1     ((volatile uint32_t *) (0x3F200004)) // GPIO Function Select 1
#define GPPUD       ((volatile uint32_t *) (0x3F200094)) // GPIO Pin Pull-up/down Enable
#define GPPUDCLK0   ((volatile uint32_t *) (0x3F200098)) // GPIO Pin Pull-up/down Enable Clock 0
#define AUX_ENABLES ((volatile uint8_t *)  (0x3F215004)) // Auxiliary Enables
#define AUX_MU_IO   ((volatile uint8_t *)  (0x3F215040)) // Mini UART I/O Data
#define AUX_MU_IER  ((volatile uint8_t *)  (0x3F215044)) // Mini UART Interrupt Enable
#define AUX_MU_IIR  ((volatile uint8_t *)  (0x3F215048)) // Mini UART Interrupt Identify
#define AUX_MU_LCR  ((volatile uint8_t *)  (0x3F21504C)) // Mini UART Line Control
#define AUX_MU_MCR  ((volatile uint8_t *)  (0x3F215050)) // Mini UART Modem Control
#define AUX_MU_LSR  ((volatile uint8_t *)  (0x3F215054)) // Mini UART Line Status
#define AUX_MU_CNTL ((volatile uint8_t *)  (0x3F215060)) // Mini UART Extra Control
#define AUX_MU_BAUD ((volatile uint16_t *) (0x3F215068)) // Mini UART Baudrate

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

void bcm2837_serial_init(void);
