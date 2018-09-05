#include <serial.h>

void serial_init()
{
    // Enable the mini UART.
    *AUX_ENABLES |= AUX_ENABLES_MU;

    // Disable interrupts.
    *AUX_MU_IER = 0;

    // Disable transmitter and receiver.
    *AUX_MU_CNTL = 0;

    // Use 8-bit mode.
    *AUX_MU_LCR = AUX_MU_LCR_8BIT;

    // Set the line to high.
    *AUX_MU_MCR = 0;

    // Set the baud rate.
    // desired_baud = clock_freq / (8 * (*aux_mu_baud + 1))
    // desired_baud = 115,200
    // clock_freq = 250,000,000
    *AUX_MU_BAUD = 270;

    // Set GPIO 14/15 to take alternative function 5.
    *GPFSEL1 = (*GPFSEL1 & GPFSEL1_FSEL14_MASK) | GPFSEL1_FSEL14_F5;
    *GPFSEL1 = (*GPFSEL1 & GPFSEL1_FSEL15_MASK) | GPFSEL1_FSEL15_F5;

    // Disable pull up/down.
    *GPPUD = 0;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Enable GPIO 14/15.
    *GPPUDCLK0 = GPPUDCLK0_PUD14 | GPPUDCLK0_PUD15;

    // Wait 150 cycles for the control signal.
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    // Flush GPIO settings.
    *GPPUDCLK0 = 0;

    // Enable the receiver and transmitter.
    *AUX_MU_CNTL = AUX_MU_CNTL_RX | AUX_MU_CNTL_TX;
    *AUX_MU_IIR = AUX_MU_IIR_RX | AUX_MU_IIR_TX;
}

char serial_getc()
{
    // Wait until receive FIFO is ready.
    while (!(*AUX_MU_LSR & AUX_MU_LSR_RX_READY));

    char r = *AUX_MU_IO;

    if (r == '\r') {
        r = '\n';
    }

    // Echo the character we received.
    serial_putc(r);

    return r;
}

void serial_putc(char c)
{
    // Wait until transmit FIFO is empty.
    while (!(*AUX_MU_LSR & AUX_MU_LSR_TX_EMPTY));

    *AUX_MU_IO = c;

    if (c == '\n') {
        serial_putc('\r');
    }
}
