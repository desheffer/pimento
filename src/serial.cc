#include <assert.h>
#include <mmio.h>
#include <serial.h>
#include <stdint.h>
#include <timer.h>

Serial* Serial::_instance = 0;

Serial::Serial()
{
    *aux_enables = 1;
    *aux_mu_ier = 0;
    *aux_mu_cntl = 0;
    *aux_mu_lcr = 3;
    *aux_mu_mcr = 0;
    *aux_mu_ier = 0;
    *aux_mu_iir = 0xC6;
    *aux_mu_baud = 270;

    uint32_t r = *gpfsel1;
    r &= ~((7 << 12) | (7 << 15));
    r |= (2 << 12) | (2 << 15);
    *gpfsel1 = r;

    *gppud = 0;
    Timer::wait(1);

    *gppudclk0 = (1 << 14) | (1 << 15);
    Timer::wait(1);

    *gppudclk0 = 0;
    *aux_mu_cntl = 3;
}

Serial::~Serial()
{
}

void Serial::init()
{
    assert(!_instance);

    _instance = new Serial();
}

char Serial::getc()
{
    while (!(*aux_mu_lsr & 0x01));

    char r = *aux_mu_io;

    return r == '\r' ? '\n' : r;
}

void Serial::putc(void*, char c)
{
    while (!(*aux_mu_lsr & 0x20));

    *aux_mu_io = c;
}
