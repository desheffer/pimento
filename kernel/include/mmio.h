#pragma once

#include <stdint.h>

#define MMIO_BASE 0x3F000000
#define MMIO_PTR(offset, size, name) \
    [[maybe_unused]] static volatile uint ## size ## _t* name = (volatile uint ## size ## _t*) (MMIO_BASE + offset);

MMIO_PTR(0x00000000,  8, peripheral_start);     // Peripheral start

MMIO_PTR(0x00003000, 32, system_timer_cs);      // System Timer Control/Status
MMIO_PTR(0x00003004, 32, system_timer_clo);     // System Timer Counter Lower 32 Bits
MMIO_PTR(0x00003008, 32, system_timer_chi);     // System Timer Counter Higher 32 Bits
MMIO_PTR(0x0000300C, 32, system_timer_c0);      // System Timer Compare 0
MMIO_PTR(0x00003010, 32, system_timer_c1);      // System Timer Compare 1
MMIO_PTR(0x00003014, 32, system_timer_c2);      // System Timer Compare 2
MMIO_PTR(0x00003018, 32, system_timer_c3);      // System Timer Compare 3

MMIO_PTR(0x0000B200, 32, irq_pending_basic);    // Pending ARM-specific IRQs
MMIO_PTR(0x0000B204, 32, irq_pending_gpu1);     // Pending IRQs 0-31
MMIO_PTR(0x0000B208, 32, irq_pending_gpu2);     // Pending IRQs 32-63
MMIO_PTR(0x0000B20C, 32, fiq_controls);         // FIQ Control
MMIO_PTR(0x0000B210, 32, irq_enable_gpu1);      // Enable IRQs 0-31
MMIO_PTR(0x0000B214, 32, irq_enable_gpu2);      // Enable IRQs 32-63
MMIO_PTR(0x0000B218, 32, irq_enable_basic);     // Enable ARM-specific IRQs
MMIO_PTR(0x0000B21C, 32, irq_disable_gpu1);     // Disable IRQs 0-31
MMIO_PTR(0x0000B220, 32, irq_disable_gpu2);     // Disable IRQs 32-63
MMIO_PTR(0x0000B224, 32, irq_disable_basic);    // Disable ARM-specific IRQs

MMIO_PTR(0x0000B400, 32, arm_timer_lod);        // ARM Timer Load
MMIO_PTR(0x0000B404, 32, arm_timer_val);        // ARM Timer Value
MMIO_PTR(0x0000B408, 32, arm_timer_ctl);        // ARM Timer Control
MMIO_PTR(0x0000B40C, 32, arm_timer_cli);        // ARM Timer IRQ Clear/Ack
MMIO_PTR(0x0000B410, 32, arm_timer_ris);        // ARM Timer Raw IRQ
MMIO_PTR(0x0000B414, 32, arm_timer_mis);        // ARM Timer Masked IRQ
MMIO_PTR(0x0000B418, 32, arm_timer_rld);        // ARM Timer Reload
MMIO_PTR(0x0000B41C, 32, arm_timer_div);        // ARM Timer Pre-divider
MMIO_PTR(0x0000B420, 32, arm_timer_cnt);        // ARM Timer Free Running Counter

MMIO_PTR(0x00200000, 32, gpfsel0);              // GPIO Function Select 0
MMIO_PTR(0x00200004, 32, gpfsel1);              // GPIO Function Select 1
MMIO_PTR(0x00200008, 32, gpfsel2);              // GPIO Function Select 2
MMIO_PTR(0x0020000C, 32, gpfsel3);              // GPIO Function Select 3
MMIO_PTR(0x00200010, 32, gpfsel4);              // GPIO Function Select 4
MMIO_PTR(0x00200014, 32, gpfsel5);              // GPIO Function Select 5
MMIO_PTR(0x0020001C, 32, gpset0);               // GPIO Pin Output Set 0
MMIO_PTR(0x00200020, 32, gpset1);               // GPIO Pin Output Set 1
MMIO_PTR(0x00200028, 32, gpclr0);               // GPIO Pin Output Clear 0
MMIO_PTR(0x0020002C, 32, gpclr1);               // GPIO Pin Output Clear 1
MMIO_PTR(0x00200034, 32, gplev0);               // GPIO Pin Level 0
MMIO_PTR(0x00200038, 32, gplev1);               // GPIO Pin Level 1
MMIO_PTR(0x00200040, 32, gpeds0);               // GPIO Pin Event Detect Status 0
MMIO_PTR(0x00200044, 32, gpeds1);               // GPIO Pin Event Detect Status 1
MMIO_PTR(0x0020004C, 32, gpren0);               // GPIO Pin Rising Edge Detect Enable 0
MMIO_PTR(0x00200050, 32, gpren1);               // GPIO Pin Rising Edge Detect Enable 1
MMIO_PTR(0x00200058, 32, gpfen0);               // GPIO Pin Falling Edge Detect Enable 0
MMIO_PTR(0x0020005C, 32, gpfen1);               // GPIO Pin Falling Edge Detect Enable 1
MMIO_PTR(0x00200064, 32, gphen0);               // GPIO Pin High Detect Enable 0
MMIO_PTR(0x00200068, 32, gphen1);               // GPIO Pin High Detect Enable 1
MMIO_PTR(0x00200070, 32, gplen0);               // GPIO Pin Low Detect Enable 0
MMIO_PTR(0x00200074, 32, gplen1);               // GPIO Pin Low Detect Enable 1
MMIO_PTR(0x0020007C, 32, gparen0);              // GPIO Pin Async. Rising Edge Detect 0
MMIO_PTR(0x00200080, 32, gparen1);              // GPIO Pin Async. Rising Edge Detect 1
MMIO_PTR(0x00200088, 32, gpafen0);              // GPIO Pin Async. Falling Edge Detect 0
MMIO_PTR(0x0020008C, 32, gpafen1);              // GPIO Pin Async. Falling Edge Detect 1
MMIO_PTR(0x00200094, 32, gppud);                // GPIO Pin Pull-up/down Enable
MMIO_PTR(0x00200098, 32, gppudclk0);            // GPIO Pin Pull-up/down Enable Clock 0
MMIO_PTR(0x0020009C, 32, gppudclk1);            // GPIO Pin Pull-up/down Enable Clock 1

MMIO_PTR(0x00215000,  8, aux_irq);              // Auxiliary Interrupt Status
MMIO_PTR(0x00215004,  8, aux_enables);          // Auxiliary Enables

MMIO_PTR(0x00215040,  8, aux_mu_io);            // Mini Uart I/O Data
MMIO_PTR(0x00215044,  8, aux_mu_ier);           // Mini Uart Interrupt Enable
MMIO_PTR(0x00215048,  8, aux_mu_iir);           // Mini Uart Interrupt Identify
MMIO_PTR(0x0021504C,  8, aux_mu_lcr);           // Mini Uart Line Control
MMIO_PTR(0x00215050,  8, aux_mu_mcr);           // Mini Uart Modem Control
MMIO_PTR(0x00215054,  8, aux_mu_lsr);           // Mini Uart Line Status
MMIO_PTR(0x00215058,  8, aux_mu_msr);           // Mini Uart Modem Status
MMIO_PTR(0x0021505C,  8, aux_mu_scratch);       // Mini Uart Scratch
MMIO_PTR(0x00215060,  8, aux_mu_cntl);          // Mini Uart Extra Control
MMIO_PTR(0x00215064, 32, aux_mu_stat);          // Mini Uart Extra Status
MMIO_PTR(0x00215068, 16, aux_mu_baud);          // Mini Uart Baudrate

MMIO_PTR(0x00215080, 32, aux_spi0_cntl0);       // SPI 1 Control register 0
MMIO_PTR(0x00215084,  8, aux_spi0_cntl1);       // SPI 1 Control register 1
MMIO_PTR(0x00215088, 32, aux_spi0_stat);        // SPI 1 Status
MMIO_PTR(0x00215090, 32, aux_spi0_io);          // SPI 1 Data
MMIO_PTR(0x00215094, 16, aux_spi0_peek);        // SPI 1 Peek
MMIO_PTR(0x002150C0, 32, aux_spi1_cntl0);       // SPI 2 Control register 0
MMIO_PTR(0x002150C4,  8, aux_spi1_cntl1);       // SPI 2 Control register 1
MMIO_PTR(0x002150C8, 32, aux_spi1_stat);        // SPI 2 Status
MMIO_PTR(0x002150D0, 32, aux_spi1_io);          // SPI 2 Data
MMIO_PTR(0x002150D4, 16, aux_spi1_peek);        // SPI 2 Peek

MMIO_PTR(0x00FFFFFF,  8, peripheral_end);       // Peripheral end
