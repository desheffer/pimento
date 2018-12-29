#include <assert.h>
#include <kstdio.h>

void do_hexdump(const char* debug, const void* src, size_t length)
{
    const char* csrc = (const char*) src;

    kputs("\n");
    kputs(debug);

    while (1) {
        kprintf("%08x%08x ", (unsigned) ((long unsigned) csrc >> 32), (unsigned) ((long unsigned) csrc));

        for (unsigned i = 0; i < 8; ++i) {
            if (i < length) {
                kprintf(" %02x", csrc[i]);
            } else {
                kputs("   ");
            }
        }

        kputs("  |");
        for (unsigned i = 0; i < 8; ++i) {
            if (i < length) {
                if (csrc[i] >= 0x20 && csrc[i] <= 0x7E) {
                    kprintf("%1c", csrc[i]);
                } else {
                    kputs(".");
                }
            }
        }
        kputs("|\n");

        if (length <= 8) {
            break;
        }

        csrc += 8;
        length -= 8;
    }

    kputs("\n");
}
