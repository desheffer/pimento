#include <mmio.h>
#include <stdint.h>
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
            asm volatile("nop");
        }

        r = *mmio::aux_mu_io;

        return r == '\r' ? '\n' : r;
    }

    void putc(char c)
    {
        while (!(*mmio::aux_mu_lsr & 0x20)) {
            asm volatile("nop");
        }

        *mmio::aux_mu_io = c;
    }

    void puts(const char* s)
    {
        while (*s) {
            if (*s == '\n') {
                putc('\r');
            }
            putc(*s++);
        }
    }

    void putx(uint32_t x)
    {
        uint32_t c;

        for (int32_t i = 32 - 4; i >= 0; i -= 4) {
            c = (x >> i) & 0xF;
            c += c > 9 ? 'A' - 10 : '0';
            putc(c);
        }
    }
}
