#include <mmio.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>
#include <uart1.h>

namespace uart1
{
    void init()
    {
        register uint32_t r;

        *mmio::aux_enables = 1;
        *mmio::aux_mu_ier = 0;
        *mmio::aux_mu_cntl = 0;
        *mmio::aux_mu_lcr = 3;
        *mmio::aux_mu_mcr = 0;
        *mmio::aux_mu_ier = 0;
        *mmio::aux_mu_iir = 0xC6;
        *mmio::aux_mu_baud = 270;

        r = *mmio::gpfsel1;
        r &= ~((7 << 12) | (7 << 15));
        r |= (2 << 12) | (2 << 15);
        *mmio::gpfsel1 = r;

        *mmio::gppud = 0;
        timer::wait(1);

        *mmio::gppudclk0 = (1 << 14) | (1 << 15);
        timer::wait(1);

        *mmio::gppudclk0 = 0;
        *mmio::aux_mu_cntl = 3;
    }

    char getc()
    {
        char r;

        while (!(*mmio::aux_mu_lsr & 0x01)) {
            timer::nop();
        }

        r = *mmio::aux_mu_io;

        return r == '\r' ? '\n' : r;
    }

    void putc(void*, char c)
    {
        while (!(*mmio::aux_mu_lsr & 0x20)) {
            timer::nop();
        }

        *mmio::aux_mu_io = c;
    }
}
