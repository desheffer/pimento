#include <assert.h>
#include <bcm2837.h>
#include <serial.h>

void serial_init()
{
    *aux_enables = 1;
    *aux_mu_ier = 0;
    *aux_mu_cntl = 0;
    *aux_mu_lcr = 3;
    *aux_mu_mcr = 0;
    *aux_mu_ier = 0;
    *aux_mu_iir = 0xC6;
    *aux_mu_baud = 270;

    unsigned r = *gpfsel1;
    r &= ~((7 << 12) | (7 << 15));
    r |= (2 << 12) | (2 << 15);
    *gpfsel1 = r;

    *gppud = 0;
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    *gppudclk0 = (1 << 14) | (1 << 15);
    for (unsigned i = 150; i > 0; --i) {
        asm volatile("nop");
    }

    *gppudclk0 = 0;
    *aux_mu_cntl = 3;
}

char serial_getc()
{
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
    while (!(*aux_mu_lsr & 0x20));

    *aux_mu_io = c;
}
