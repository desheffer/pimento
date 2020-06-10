#pragma once

#include <stdint.h>

#define BCM2837_GPFSEL1     ((volatile uint32_t *) (0xFFFF00003F200004)) // GPIO Function Select 1
#define BCM2837_GPPUD       ((volatile uint32_t *) (0xFFFF00003F200094)) // GPIO Pin Pull-up/down Enable
#define BCM2837_GPPUDCLK0   ((volatile uint32_t *) (0xFFFF00003F200098)) // GPIO Pin Pull-up/down Enable Clock 0
#define BCM2837_AUX_ENABLES ((volatile uint8_t *)  (0xFFFF00003F215004)) // Auxiliary Enables
#define BCM2837_AUX_MU_IO   ((volatile uint8_t *)  (0xFFFF00003F215040)) // Mini UART I/O Data
#define BCM2837_AUX_MU_IER  ((volatile uint8_t *)  (0xFFFF00003F215044)) // Mini UART Interrupt Enable
#define BCM2837_AUX_MU_IIR  ((volatile uint8_t *)  (0xFFFF00003F215048)) // Mini UART Interrupt Identify
#define BCM2837_AUX_MU_LCR  ((volatile uint8_t *)  (0xFFFF00003F21504C)) // Mini UART Line Control
#define BCM2837_AUX_MU_MCR  ((volatile uint8_t *)  (0xFFFF00003F215050)) // Mini UART Modem Control
#define BCM2837_AUX_MU_LSR  ((volatile uint8_t *)  (0xFFFF00003F215054)) // Mini UART Line Status
#define BCM2837_AUX_MU_CNTL ((volatile uint8_t *)  (0xFFFF00003F215060)) // Mini UART Extra Control
#define BCM2837_AUX_MU_BAUD ((volatile uint16_t *) (0xFFFF00003F215068)) // Mini UART Baudrate

#define BCM2837_AUX_ENABLES_MU (0b1 << 0) // Enable mini UART

#define BCM2837_AUX_MU_LCR_8BIT (0b11 << 0) // Enable 8-bit mode

#define BCM2837_GPFSEL1_FSEL14_MASK (~(0b111 << 12)) // Clear GPIO 14 function
#define BCM2837_GPFSEL1_FSEL15_MASK (~(0b111 << 15)) // Clear GPIO 15 function

#define BCM2837_GPFSEL1_FSEL14_F5 (0b010 << 12) // Set GPIO 14 to take function 5
#define BCM2837_GPFSEL1_FSEL15_F5 (0b010 << 15) // Set GPIO 15 to take function 5

#define BCM2837_GPPUDCLK0_PUD14 (0b1 << 14) // Enable GPIO 14
#define BCM2837_GPPUDCLK0_PUD15 (0b1 << 15) // Enable GPIO 15

#define BCM2837_AUX_MU_CNTL_RX (0b1 << 0) // Enable RX
#define BCM2837_AUX_MU_CNTL_TX (0b1 << 1) // Enable TX

#define BCM2837_AUX_MU_IIR_TX (0b1 << 0) // Clear TX
#define BCM2837_AUX_MU_IIR_RX (0b1 << 1) // Clear RX

#define BCM2837_AUX_MU_LSR_RX_READY (0b1 << 0) // Data ready
#define BCM2837_AUX_MU_LSR_TX_EMPTY (0b1 << 5) // Transmitter empty

void bcm2837_serial_init(void);
