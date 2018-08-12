#include <assert.h>
#include <mmio.h>
#include <serial.h>

Serial::Serial()
{
}

Serial::~Serial()
{
}

void Serial::init()
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

char Serial::getc()
{
    while (!(*aux_mu_lsr & 0x01));

    char r = *aux_mu_io;
    putc(r);

    return r == '\r' ? '\n' : r;
}

void Serial::putc(char c)
{
    while (!(*aux_mu_lsr & 0x20));

    *aux_mu_io = c;
}
