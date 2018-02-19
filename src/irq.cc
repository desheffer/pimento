#include <irq.h>
#include <mmio.h>
#include <stdint.h>

namespace irq
{
    static handler_t handlers[NUM_IRQS];
    static clearer_t clearers[NUM_IRQS];

    inline void enable()
    {
        asm volatile("msr daifclr, #2");
    }

    inline void disable()
    {
        asm volatile("msr daifset, #2");
    }

    void init()
    {
        *mmio::irq_disable_gpu1 = 0xFFFFFFFF;
        *mmio::irq_disable_gpu2 = 0xFFFFFFFF;
        *mmio::irq_disable_basic = 0xFFFFFFFF;

        enable();
    }

    void register_handler(irq_number_t num, handler_t handler, clearer_t clearer)
    {
        handlers[num] = handler;
        clearers[num] = clearer;

        if (num < 32) {
            *mmio::irq_enable_gpu1 |= (1 << (num - 0));
        } else if (num < 64) {
            *mmio::irq_enable_gpu2 |= (1 << (num - 32));
        } else {
            *mmio::irq_enable_basic |= (1 << (num - 64));
        }
    }

    inline bool irq_pending(uint32_t num)
    {
        if (num < 32) {
            return *mmio::irq_pending_gpu1 & (1 << (num - 0));
        } else if (num < 64) {
            return *mmio::irq_pending_gpu2 & (1 << (num - 32));
        } else {
            return *mmio::irq_pending_basic & (1 << (num - 64));
        }
    }

    void irq_handler()
    {
        uint32_t num;

        for (num = 0; num < NUM_IRQS; num++) {
            if (irq_pending(num) && irq::handlers[num]) {
                irq::clearers[num]();
                irq::handlers[num]();
                break;
            }
        }
    }
}
