#pragma once

#include <stdint.h>

#define MEM(address, size) ((volatile uint ## size ## _t*) address)

#define peripheral_start        MEM(0x3F000000,  8)     // Peripheral Start

#define system_timer_cs         MEM(0x3F003000, 32)     // System Timer Control/Status
#define system_timer_clo        MEM(0x3F003004, 32)     // System Timer Counter Lower 32 Bits
#define system_timer_chi        MEM(0x3F003008, 32)     // System Timer Counter Higher 32 Bits
#define system_timer_c0         MEM(0x3F00300C, 32)     // System Timer Compare 0
#define system_timer_c1         MEM(0x3F003010, 32)     // System Timer Compare 1
#define system_timer_c2         MEM(0x3F003014, 32)     // System Timer Compare 2
#define system_timer_c3         MEM(0x3F003018, 32)     // System Timer Compare 3

#define irq_pending_basic       MEM(0x3F00B200, 32)     // Pending ARM-specific IRQs
#define irq_pending_gpu1        MEM(0x3F00B204, 32)     // Pending IRQs 0-31
#define irq_pending_gpu2        MEM(0x3F00B208, 32)     // Pending IRQs 32-63
#define fiq_controls            MEM(0x3F00B20C, 32)     // FIQ Control
#define irq_enable_gpu1         MEM(0x3F00B210, 32)     // Enable IRQs 0-31
#define irq_enable_gpu2         MEM(0x3F00B214, 32)     // Enable IRQs 32-63
#define irq_enable_basic        MEM(0x3F00B218, 32)     // Enable ARM-specific IRQs
#define irq_disable_gpu1        MEM(0x3F00B21C, 32)     // Disable IRQs 0-31
#define irq_disable_gpu2        MEM(0x3F00B220, 32)     // Disable IRQs 32-63
#define irq_disable_basic       MEM(0x3F00B224, 32)     // Disable ARM-specific IRQs

#define arm_timer_lod           MEM(0x3F00B400, 32)     // ARM Timer Load
#define arm_timer_val           MEM(0x3F00B404, 32)     // ARM Timer Value
#define arm_timer_ctl           MEM(0x3F00B408, 32)     // ARM Timer Control
#define arm_timer_cli           MEM(0x3F00B40C, 32)     // ARM Timer IRQ Clear/Ack
#define arm_timer_ris           MEM(0x3F00B410, 32)     // ARM Timer Raw IRQ
#define arm_timer_mis           MEM(0x3F00B414, 32)     // ARM Timer Masked IRQ
#define arm_timer_rld           MEM(0x3F00B418, 32)     // ARM Timer Reload
#define arm_timer_div           MEM(0x3F00B41C, 32)     // ARM Timer Pre-divider
#define arm_timer_cnt           MEM(0x3F00B420, 32)     // ARM Timer Free Running Counter

#define gpfsel0                 MEM(0x3F200000, 32)     // GPIO Function Select 0
#define gpfsel1                 MEM(0x3F200004, 32)     // GPIO Function Select 1
#define gpfsel2                 MEM(0x3F200008, 32)     // GPIO Function Select 2
#define gpfsel3                 MEM(0x3F20000C, 32)     // GPIO Function Select 3
#define gpfsel4                 MEM(0x3F200010, 32)     // GPIO Function Select 4
#define gpfsel5                 MEM(0x3F200014, 32)     // GPIO Function Select 5
#define gpset0                  MEM(0x3F20001C, 32)     // GPIO Pin Output Set 0
#define gpset1                  MEM(0x3F200020, 32)     // GPIO Pin Output Set 1
#define gpclr0                  MEM(0x3F200028, 32)     // GPIO Pin Output Clear 0
#define gpclr1                  MEM(0x3F20002C, 32)     // GPIO Pin Output Clear 1
#define gplev0                  MEM(0x3F200034, 32)     // GPIO Pin Level 0
#define gplev1                  MEM(0x3F200038, 32)     // GPIO Pin Level 1
#define gpeds0                  MEM(0x3F200040, 32)     // GPIO Pin Event Detect Status 0
#define gpeds1                  MEM(0x3F200044, 32)     // GPIO Pin Event Detect Status 1
#define gpren0                  MEM(0x3F20004C, 32)     // GPIO Pin Rising Edge Detect Enable 0
#define gpren1                  MEM(0x3F200050, 32)     // GPIO Pin Rising Edge Detect Enable 1
#define gpfen0                  MEM(0x3F200058, 32)     // GPIO Pin Falling Edge Detect Enable 0
#define gpfen1                  MEM(0x3F20005C, 32)     // GPIO Pin Falling Edge Detect Enable 1
#define gphen0                  MEM(0x3F200064, 32)     // GPIO Pin High Detect Enable 0
#define gphen1                  MEM(0x3F200068, 32)     // GPIO Pin High Detect Enable 1
#define gplen0                  MEM(0x3F200070, 32)     // GPIO Pin Low Detect Enable 0
#define gplen1                  MEM(0x3F200074, 32)     // GPIO Pin Low Detect Enable 1
#define gparen0                 MEM(0x3F20007C, 32)     // GPIO Pin Async. Rising Edge Detect 0
#define gparen1                 MEM(0x3F200080, 32)     // GPIO Pin Async. Rising Edge Detect 1
#define gpafen0                 MEM(0x3F200088, 32)     // GPIO Pin Async. Falling Edge Detect 0
#define gpafen1                 MEM(0x3F20008C, 32)     // GPIO Pin Async. Falling Edge Detect 1
#define gppud                   MEM(0x3F200094, 32)     // GPIO Pin Pull-up/down Enable
#define gppudclk0               MEM(0x3F200098, 32)     // GPIO Pin Pull-up/down Enable Clock 0
#define gppudclk1               MEM(0x3F20009C, 32)     // GPIO Pin Pull-up/down Enable Clock 1

#define aux_irq                 MEM(0x3F215000,  8)     // Auxiliary Interrupt Status
#define aux_enables             MEM(0x3F215004,  8)     // Auxiliary Enables

#define aux_mu_io               MEM(0x3F215040,  8)     // Mini UART I/O Data
#define aux_mu_ier              MEM(0x3F215044,  8)     // Mini UART Interrupt Enable
#define aux_mu_iir              MEM(0x3F215048,  8)     // Mini UART Interrupt Identify
#define aux_mu_lcr              MEM(0x3F21504C,  8)     // Mini UART Line Control
#define aux_mu_mcr              MEM(0x3F215050,  8)     // Mini UART Modem Control
#define aux_mu_lsr              MEM(0x3F215054,  8)     // Mini UART Line Status
#define aux_mu_msr              MEM(0x3F215058,  8)     // Mini UART Modem Status
#define aux_mu_scratch          MEM(0x3F21505C,  8)     // Mini UART Scratch
#define aux_mu_cntl             MEM(0x3F215060,  8)     // Mini UART Extra Control
#define aux_mu_stat             MEM(0x3F215064, 32)     // Mini UART Extra Status
#define aux_mu_baud             MEM(0x3F215068, 16)     // Mini UART Baudrate

#define aux_spi0_cntl0          MEM(0x3F215080, 32)     // SPI 1 Control Register 0
#define aux_spi0_cntl1          MEM(0x3F215084,  8)     // SPI 1 Control Register 1
#define aux_spi0_stat           MEM(0x3F215088, 32)     // SPI 1 Status
#define aux_spi0_io             MEM(0x3F215090, 32)     // SPI 1 Data
#define aux_spi0_peek           MEM(0x3F215094, 16)     // SPI 1 Peek
#define aux_spi1_cntl0          MEM(0x3F2150C0, 32)     // SPI 2 Control Register 0
#define aux_spi1_cntl1          MEM(0x3F2150C4,  8)     // SPI 2 Control Register 1
#define aux_spi1_stat           MEM(0x3F2150C8, 32)     // SPI 2 Status
#define aux_spi1_io             MEM(0x3F2150D0, 32)     // SPI 2 Data
#define aux_spi1_peek           MEM(0x3F2150D4, 16)     // SPI 2 Peek

#define core_timers_control     MEM(0x40000000, 32)     // Control register
#define core_timers_prescaler   MEM(0x40000008, 32)     // Core timer prescaler
#define core0_timers_cntl       MEM(0x40000040, 32)     // Core 0 Timers Interrupt control
#define core1_timers_cntl       MEM(0x40000044, 32)     // Core 1 Timers Interrupt control
#define core2_timers_cntl       MEM(0x40000048, 32)     // Core 2 Timers Interrupt control
#define core3_timers_cntl       MEM(0x4000004C, 32)     // Core 3 Timers Interrupt control
#define core0_mbox_cntl         MEM(0x40000050, 32)     // Core 0 Mailboxes Interrupt control
#define core1_mbox_cntl         MEM(0x40000054, 32)     // Core 1 Mailboxes Interrupt control
#define core2_mbox_cntl         MEM(0x40000058, 32)     // Core 2 Mailboxes Interrupt control
#define core3_mbox_cntl         MEM(0x4000005C, 32)     // Core 3 Mailboxes Interrupt control
#define core0_irq               MEM(0x40000060, 32)     // Core 0 IRQ Source
#define core1_irq               MEM(0x40000064, 32)     // Core 1 IRQ Source
#define core2_irq               MEM(0x40000068, 32)     // Core 2 IRQ Source
#define core3_irq               MEM(0x4000006C, 32)     // Core 3 IRQ Source
#define core0_fiq               MEM(0x40000070, 32)     // Core 0 FIQ Source
#define core1_fiq               MEM(0x40000074, 32)     // Core 1 FIQ Source
#define core2_fiq               MEM(0x40000078, 32)     // Core 2 FIQ Source
#define core3_fiq               MEM(0x4000007C, 32)     // Core 3 FIQ Source
#define core0_mbox0_w           MEM(0x40000080, 32)     // Core 0 Mailbox 0 Write-set (WO)
#define core0_mbox1_w           MEM(0x40000084, 32)     // Core 0 Mailbox 1 Write-set (WO)
#define core0_mbox2_w           MEM(0x40000088, 32)     // Core 0 Mailbox 2 Write-set (WO)
#define core0_mbox3_w           MEM(0x4000008C, 32)     // Core 0 Mailbox 3 Write-set (WO)
#define core1_mbox0_w           MEM(0x40000090, 32)     // Core 1 Mailbox 0 Write-set (WO)
#define core1_mbox1_w           MEM(0x40000094, 32)     // Core 1 Mailbox 1 Write-set (WO)
#define core1_mbox2_w           MEM(0x40000098, 32)     // Core 1 Mailbox 2 Write-set (WO)
#define core1_mbox3_w           MEM(0x4000009C, 32)     // Core 1 Mailbox 3 Write-set (WO)
#define core2_mbox0_w           MEM(0x400000A0, 32)     // Core 2 Mailbox 0 Write-set (WO)
#define core2_mbox1_w           MEM(0x400000A4, 32)     // Core 2 Mailbox 1 Write-set (WO)
#define core2_mbox2_w           MEM(0x400000A8, 32)     // Core 2 Mailbox 2 Write-set (WO)
#define core2_mbox3_w           MEM(0x400000AC, 32)     // Core 2 Mailbox 3 Write-set (WO)
#define core3_mbox0_w           MEM(0x400000B0, 32)     // Core 3 Mailbox 0 Write-set (WO)
#define core3_mbox1_w           MEM(0x400000B4, 32)     // Core 3 Mailbox 1 Write-set (WO)
#define core3_mbox2_w           MEM(0x400000B8, 32)     // Core 3 Mailbox 2 Write-set (WO)
#define core3_mbox3_w           MEM(0x400000BC, 32)     // Core 3 Mailbox 3 Write-set (WO)
#define core0_mbox0_rw          MEM(0x400000C0, 32)     // Core 0 Mailbox 0 Read & Write-high-to-clear
#define core0_mbox1_rw          MEM(0x400000C4, 32)     // Core 0 Mailbox 1 Read & Write-high-to-clear
#define core0_mbox2_rw          MEM(0x400000C8, 32)     // Core 0 Mailbox 2 Read & Write-high-to-clear
#define core0_mbox3_rw          MEM(0x400000CC, 32)     // Core 0 Mailbox 3 Read & Write-high-to-clear
#define core1_mbox0_rw          MEM(0x400000D0, 32)     // Core 1 Mailbox 0 Read & Write-high-to-clear
#define core1_mbox1_rw          MEM(0x400000D4, 32)     // Core 1 Mailbox 1 Read & Write-high-to-clear
#define core1_mbox2_rw          MEM(0x400000D8, 32)     // Core 1 Mailbox 2 Read & Write-high-to-clear
#define core1_mbox3_rw          MEM(0x400000DC, 32)     // Core 1 Mailbox 3 Read & Write-high-to-clear
#define core2_mbox0_rw          MEM(0x400000E0, 32)     // Core 2 Mailbox 0 Read & Write-high-to-clear
#define core2_mbox1_rw          MEM(0x400000E4, 32)     // Core 2 Mailbox 1 Read & Write-high-to-clear
#define core2_mbox2_rw          MEM(0x400000E8, 32)     // Core 2 Mailbox 2 Read & Write-high-to-clear
#define core2_mbox3_rw          MEM(0x400000EC, 32)     // Core 2 Mailbox 3 Read & Write-high-to-clear
#define core3_mbox0_rw          MEM(0x400000F0, 32)     // Core 3 Mailbox 0 Read & Write-high-to-clear
#define core3_mbox1_rw          MEM(0x400000F4, 32)     // Core 3 Mailbox 1 Read & Write-high-to-clear
#define core3_mbox2_rw          MEM(0x400000F8, 32)     // Core 3 Mailbox 2 Read & Write-high-to-clear
#define core3_mbox3_rw          MEM(0x400000FC, 32)     // Core 3 Mailbox 3 Read & Write-high-to-clear

#define peripheral_end          MEM(0x400000FC,  8)     // Peripheral End
