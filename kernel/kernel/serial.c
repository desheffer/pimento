#include <bcm2837.h>
#include <serial.h>

void serial_init()
{
    // Enable the mini UART.
    *aux_enables |= 0x1;

    // Disable interrupts.
    *aux_mu_ier = 0;

    // Disable transmitter and receiver.
    *aux_mu_cntl = 0;

    // Use 8-bit mode.
    *aux_mu_lcr = 0x3;

    // Set the line to high.
    *aux_mu_mcr = 0;

    // Set the baud rate.
    // desired_baud = clock_freq / (8 * (*aux_mu_baud + 1))
    // desired_baud = 115,200
    // clock_freq = 250,000,000
    *aux_mu_baud = 270;

    // Set GPIO 14 (bits 12-14) to take alternative function 5 (0b010).
    *gpfsel1 = (*gpfsel1 & ~(0b111 << 12)) | (0b010 << 12);

    // Set GPIO 15 (bits 15-17) to take alternative function 5 (0b010).
    *gpfsel1 = (*gpfsel1 & ~(0b111 << 15)) | (0b010 << 15);

    // Disable pull up/down.
    *gppud = 0;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Enable GPIO 14 and GPIO 15.
    *gppudclk0 = (0x1 << 14) | (0x1 << 15);

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Flush GPIO settings.
    *gppudclk0 = 0;

    // Enable the transmitter and receiver.
    *aux_mu_cntl = 0x3;

    // Clear the receive and transmit FIFOs.
    *aux_mu_iir = 0x6;
}

char serial_getc()
{
    // Wait until receive FIFO is not empty.
    while (!(*aux_mu_lsr & 0x01));

    char r = *aux_mu_io;

    if (r == '\r') {
        r = '\n';
    }

    serial_putc(r);

    return r;
}

void serial_putc(char c)
{
    // Wait until transmit FIFO is empty.
    while (!(*aux_mu_lsr & 0x20));

    *aux_mu_io = c;

    if (c == '\n') {
        serial_putc('\r');
    }
}
