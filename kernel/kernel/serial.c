#include <bcm2837.h>
#include <serial.h>

void serial_init()
{
    // Enable the mini UART.
    *AUX_ENABLES |= 0x1;

    // Disable interrupts.
    *AUX_MU_IER = 0;

    // Disable transmitter and receiver.
    *AUX_MU_CNTL = 0;

    // Use 8-bit mode.
    *AUX_MU_LCR = 0x3;

    // Set the line to high.
    *AUX_MU_MCR = 0;

    // Set the baud rate.
    // desired_baud = clock_freq / (8 * (*aux_mu_baud + 1))
    // desired_baud = 115,200
    // clock_freq = 250,000,000
    *AUX_MU_BAUD = 270;

    // Set GPIO 14 (bits 12-14) to take alternative function 5 (0b010).
    *GPFSEL1 = (*GPFSEL1 & ~(0b111 << 12)) | (0b010 << 12);

    // Set GPIO 15 (bits 15-17) to take alternative function 5 (0b010).
    *GPFSEL1 = (*GPFSEL1 & ~(0b111 << 15)) | (0b010 << 15);

    // Disable pull up/down.
    *GPPUD = 0;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Enable GPIO 14 and GPIO 15.
    *GPPUDCLK0 = (0x1 << 14) | (0x1 << 15);

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Flush GPIO settings.
    *GPPUDCLK0 = 0;

    // Enable the transmitter and receiver.
    *AUX_MU_CNTL = 0x3;

    // Clear the receive and transmit FIFOs.
    *AUX_MU_IIR = 0x6;
}

char serial_getc()
{
    // Wait until receive FIFO is not empty.
    while (!(*AUX_MU_LSR & 0x01));

    char r = *AUX_MU_IO;

    if (r == '\r') {
        r = '\n';
    }

    serial_putc(r);

    return r;
}

void serial_putc(char c)
{
    // Wait until transmit FIFO is empty.
    while (!(*AUX_MU_LSR & 0x20));

    *AUX_MU_IO = c;

    if (c == '\n') {
        serial_putc('\r');
    }
}
