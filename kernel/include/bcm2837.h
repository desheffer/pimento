#pragma once

#include <memory.h>
#include <stdint.h>

#define MEM(address, size) ((volatile uint ## size ## _t*) ((address) | VA_START))

#define PERIPHERAL_START        MEM(0x3F000000,  8)     // Peripheral Start

#define SYSTEM_TIMER_CS         MEM(0x3F003000, 32)     // System Timer Control/Status
#define SYSTEM_TIMER_CLO        MEM(0x3F003004, 32)     // System Timer Counter Lower 32 Bits
#define SYSTEM_TIMER_CHI        MEM(0x3F003008, 32)     // System Timer Counter Higher 32 Bits
#define SYSTEM_TIMER_C0         MEM(0x3F00300C, 32)     // System Timer Compare 0
#define SYSTEM_TIMER_C1         MEM(0x3F003010, 32)     // System Timer Compare 1
#define SYSTEM_TIMER_C2         MEM(0x3F003014, 32)     // System Timer Compare 2
#define SYSTEM_TIMER_C3         MEM(0x3F003018, 32)     // System Timer Compare 3

#define IRQ_PENDING_BASIC       MEM(0x3F00B200, 32)     // Pending ARM-specific IRQs
#define IRQ_PENDING_GPU1        MEM(0x3F00B204, 32)     // Pending IRQs 0-31
#define IRQ_PENDING_GPU2        MEM(0x3F00B208, 32)     // Pending IRQs 32-63
#define FIQ_CONTROLS            MEM(0x3F00B20C, 32)     // FIQ Control
#define IRQ_ENABLE_GPU1         MEM(0x3F00B210, 32)     // Enable IRQs 0-31
#define IRQ_ENABLE_GPU2         MEM(0x3F00B214, 32)     // Enable IRQs 32-63
#define IRQ_ENABLE_BASIC        MEM(0x3F00B218, 32)     // Enable ARM-specific IRQs
#define IRQ_DISABLE_GPU1        MEM(0x3F00B21C, 32)     // Disable IRQs 0-31
#define IRQ_DISABLE_GPU2        MEM(0x3F00B220, 32)     // Disable IRQs 32-63
#define IRQ_DISABLE_BASIC       MEM(0x3F00B224, 32)     // Disable ARM-specific IRQs

#define ARM_TIMER_LOD           MEM(0x3F00B400, 32)     // ARM Timer Load
#define ARM_TIMER_VAL           MEM(0x3F00B404, 32)     // ARM Timer Value
#define ARM_TIMER_CTL           MEM(0x3F00B408, 32)     // ARM Timer Control
#define ARM_TIMER_CLI           MEM(0x3F00B40C, 32)     // ARM Timer IRQ Clear/Ack
#define ARM_TIMER_RIS           MEM(0x3F00B410, 32)     // ARM Timer Raw IRQ
#define ARM_TIMER_MIS           MEM(0x3F00B414, 32)     // ARM Timer Masked IRQ
#define ARM_TIMER_RLD           MEM(0x3F00B418, 32)     // ARM Timer Reload
#define ARM_TIMER_DIV           MEM(0x3F00B41C, 32)     // ARM Timer Pre-divider
#define ARM_TIMER_CNT           MEM(0x3F00B420, 32)     // ARM Timer Free Running Counter

#define GPFSEL0                 MEM(0x3F200000, 32)     // GPIO Function Select 0
#define GPFSEL1                 MEM(0x3F200004, 32)     // GPIO Function Select 1
#define GPFSEL2                 MEM(0x3F200008, 32)     // GPIO Function Select 2
#define GPFSEL3                 MEM(0x3F20000C, 32)     // GPIO Function Select 3
#define GPFSEL4                 MEM(0x3F200010, 32)     // GPIO Function Select 4
#define GPFSEL5                 MEM(0x3F200014, 32)     // GPIO Function Select 5
#define GPSET0                  MEM(0x3F20001C, 32)     // GPIO Pin Output Set 0
#define GPSET1                  MEM(0x3F200020, 32)     // GPIO Pin Output Set 1
#define GPCLR0                  MEM(0x3F200028, 32)     // GPIO Pin Output Clear 0
#define GPCLR1                  MEM(0x3F20002C, 32)     // GPIO Pin Output Clear 1
#define GPLEV0                  MEM(0x3F200034, 32)     // GPIO Pin Level 0
#define GPLEV1                  MEM(0x3F200038, 32)     // GPIO Pin Level 1
#define GPEDS0                  MEM(0x3F200040, 32)     // GPIO Pin Event Detect Status 0
#define GPEDS1                  MEM(0x3F200044, 32)     // GPIO Pin Event Detect Status 1
#define GPREN0                  MEM(0x3F20004C, 32)     // GPIO Pin Rising Edge Detect Enable 0
#define GPREN1                  MEM(0x3F200050, 32)     // GPIO Pin Rising Edge Detect Enable 1
#define GPFEN0                  MEM(0x3F200058, 32)     // GPIO Pin Falling Edge Detect Enable 0
#define GPFEN1                  MEM(0x3F20005C, 32)     // GPIO Pin Falling Edge Detect Enable 1
#define GPHEN0                  MEM(0x3F200064, 32)     // GPIO Pin High Detect Enable 0
#define GPHEN1                  MEM(0x3F200068, 32)     // GPIO Pin High Detect Enable 1
#define GPLEN0                  MEM(0x3F200070, 32)     // GPIO Pin Low Detect Enable 0
#define GPLEN1                  MEM(0x3F200074, 32)     // GPIO Pin Low Detect Enable 1
#define GPAREN0                 MEM(0x3F20007C, 32)     // GPIO Pin Async. Rising Edge Detect 0
#define GPAREN1                 MEM(0x3F200080, 32)     // GPIO Pin Async. Rising Edge Detect 1
#define GPAFEN0                 MEM(0x3F200088, 32)     // GPIO Pin Async. Falling Edge Detect 0
#define GPAFEN1                 MEM(0x3F20008C, 32)     // GPIO Pin Async. Falling Edge Detect 1
#define GPPUD                   MEM(0x3F200094, 32)     // GPIO Pin Pull-up/down Enable
#define GPPUDCLK0               MEM(0x3F200098, 32)     // GPIO Pin Pull-up/down Enable Clock 0
#define GPPUDCLK1               MEM(0x3F20009C, 32)     // GPIO Pin Pull-up/down Enable Clock 1

#define AUX_IRQ                 MEM(0x3F215000,  8)     // Auxiliary Interrupt Status
#define AUX_ENABLES             MEM(0x3F215004,  8)     // Auxiliary Enables

#define AUX_MU_IO               MEM(0x3F215040,  8)     // Mini UART I/O Data
#define AUX_MU_IER              MEM(0x3F215044,  8)     // Mini UART Interrupt Enable
#define AUX_MU_IIR              MEM(0x3F215048,  8)     // Mini UART Interrupt Identify
#define AUX_MU_LCR              MEM(0x3F21504C,  8)     // Mini UART Line Control
#define AUX_MU_MCR              MEM(0x3F215050,  8)     // Mini UART Modem Control
#define AUX_MU_LSR              MEM(0x3F215054,  8)     // Mini UART Line Status
#define AUX_MU_MSR              MEM(0x3F215058,  8)     // Mini UART Modem Status
#define AUX_MU_SCRATCH          MEM(0x3F21505C,  8)     // Mini UART Scratch
#define AUX_MU_CNTL             MEM(0x3F215060,  8)     // Mini UART Extra Control
#define AUX_MU_STAT             MEM(0x3F215064, 32)     // Mini UART Extra Status
#define AUX_MU_BAUD             MEM(0x3F215068, 16)     // Mini UART Baudrate

#define AUX_SPI0_CNTL0          MEM(0x3F215080, 32)     // SPI 1 Control Register 0
#define AUX_SPI0_CNTL1          MEM(0x3F215084,  8)     // SPI 1 Control Register 1
#define AUX_SPI0_STAT           MEM(0x3F215088, 32)     // SPI 1 Status
#define AUX_SPI0_IO             MEM(0x3F215090, 32)     // SPI 1 Data
#define AUX_SPI0_PEEK           MEM(0x3F215094, 16)     // SPI 1 Peek
#define AUX_SPI1_CNTL0          MEM(0x3F2150C0, 32)     // SPI 2 Control Register 0
#define AUX_SPI1_CNTL1          MEM(0x3F2150C4,  8)     // SPI 2 Control Register 1
#define AUX_SPI1_STAT           MEM(0x3F2150C8, 32)     // SPI 2 Status
#define AUX_SPI1_IO             MEM(0x3F2150D0, 32)     // SPI 2 Data
#define AUX_SPI1_PEEK           MEM(0x3F2150D4, 16)     // SPI 2 Peek

#define PERIPHERAL_END          MEM(0x40000000,  8)     // Peripheral End

#define CORE_TIMERS_CONTROL     MEM(0x40000000, 32)     // Control Register
#define CORE_TIMERS_PRESCALER   MEM(0x40000008, 32)     // Core Timer Prescaler
#define CORE0_TIMERS_CNTL       MEM(0x40000040, 32)     // Core 0 Timers Interrupt control
#define CORE1_TIMERS_CNTL       MEM(0x40000044, 32)     // Core 1 Timers Interrupt control
#define CORE2_TIMERS_CNTL       MEM(0x40000048, 32)     // Core 2 Timers Interrupt control
#define CORE3_TIMERS_CNTL       MEM(0x4000004C, 32)     // Core 3 Timers Interrupt control
#define CORE0_MBOX_CNTL         MEM(0x40000050, 32)     // Core 0 Mailboxes Interrupt control
#define CORE1_MBOX_CNTL         MEM(0x40000054, 32)     // Core 1 Mailboxes Interrupt control
#define CORE2_MBOX_CNTL         MEM(0x40000058, 32)     // Core 2 Mailboxes Interrupt control
#define CORE3_MBOX_CNTL         MEM(0x4000005C, 32)     // Core 3 Mailboxes Interrupt control
#define CORE0_IRQ               MEM(0x40000060, 32)     // Core 0 IRQ Source
#define CORE1_IRQ               MEM(0x40000064, 32)     // Core 1 IRQ Source
#define CORE2_IRQ               MEM(0x40000068, 32)     // Core 2 IRQ Source
#define CORE3_IRQ               MEM(0x4000006C, 32)     // Core 3 IRQ Source
#define CORE0_FIQ               MEM(0x40000070, 32)     // Core 0 FIQ Source
#define CORE1_FIQ               MEM(0x40000074, 32)     // Core 1 FIQ Source
#define CORE2_FIQ               MEM(0x40000078, 32)     // Core 2 FIQ Source
#define CORE3_FIQ               MEM(0x4000007C, 32)     // Core 3 FIQ Source
#define CORE0_MBOX0_W           MEM(0x40000080, 32)     // Core 0 Mailbox 0 Write-set (WO)
#define CORE0_MBOX1_W           MEM(0x40000084, 32)     // Core 0 Mailbox 1 Write-set (WO)
#define CORE0_MBOX2_W           MEM(0x40000088, 32)     // Core 0 Mailbox 2 Write-set (WO)
#define CORE0_MBOX3_W           MEM(0x4000008C, 32)     // Core 0 Mailbox 3 Write-set (WO)
#define CORE1_MBOX0_W           MEM(0x40000090, 32)     // Core 1 Mailbox 0 Write-set (WO)
#define CORE1_MBOX1_W           MEM(0x40000094, 32)     // Core 1 Mailbox 1 Write-set (WO)
#define CORE1_MBOX2_W           MEM(0x40000098, 32)     // Core 1 Mailbox 2 Write-set (WO)
#define CORE1_MBOX3_W           MEM(0x4000009C, 32)     // Core 1 Mailbox 3 Write-set (WO)
#define CORE2_MBOX0_W           MEM(0x400000A0, 32)     // Core 2 Mailbox 0 Write-set (WO)
#define CORE2_MBOX1_W           MEM(0x400000A4, 32)     // Core 2 Mailbox 1 Write-set (WO)
#define CORE2_MBOX2_W           MEM(0x400000A8, 32)     // Core 2 Mailbox 2 Write-set (WO)
#define CORE2_MBOX3_W           MEM(0x400000AC, 32)     // Core 2 Mailbox 3 Write-set (WO)
#define CORE3_MBOX0_W           MEM(0x400000B0, 32)     // Core 3 Mailbox 0 Write-set (WO)
#define CORE3_MBOX1_W           MEM(0x400000B4, 32)     // Core 3 Mailbox 1 Write-set (WO)
#define CORE3_MBOX2_W           MEM(0x400000B8, 32)     // Core 3 Mailbox 2 Write-set (WO)
#define CORE3_MBOX3_W           MEM(0x400000BC, 32)     // Core 3 Mailbox 3 Write-set (WO)
#define CORE0_MBOX0_RW          MEM(0x400000C0, 32)     // Core 0 Mailbox 0 Read & Write-high-to-clear
#define CORE0_MBOX1_RW          MEM(0x400000C4, 32)     // Core 0 Mailbox 1 Read & Write-high-to-clear
#define CORE0_MBOX2_RW          MEM(0x400000C8, 32)     // Core 0 Mailbox 2 Read & Write-high-to-clear
#define CORE0_MBOX3_RW          MEM(0x400000CC, 32)     // Core 0 Mailbox 3 Read & Write-high-to-clear
#define CORE1_MBOX0_RW          MEM(0x400000D0, 32)     // Core 1 Mailbox 0 Read & Write-high-to-clear
#define CORE1_MBOX1_RW          MEM(0x400000D4, 32)     // Core 1 Mailbox 1 Read & Write-high-to-clear
#define CORE1_MBOX2_RW          MEM(0x400000D8, 32)     // Core 1 Mailbox 2 Read & Write-high-to-clear
#define CORE1_MBOX3_RW          MEM(0x400000DC, 32)     // Core 1 Mailbox 3 Read & Write-high-to-clear
#define CORE2_MBOX0_RW          MEM(0x400000E0, 32)     // Core 2 Mailbox 0 Read & Write-high-to-clear
#define CORE2_MBOX1_RW          MEM(0x400000E4, 32)     // Core 2 Mailbox 1 Read & Write-high-to-clear
#define CORE2_MBOX2_RW          MEM(0x400000E8, 32)     // Core 2 Mailbox 2 Read & Write-high-to-clear
#define CORE2_MBOX3_RW          MEM(0x400000EC, 32)     // Core 2 Mailbox 3 Read & Write-high-to-clear
#define CORE3_MBOX0_RW          MEM(0x400000F0, 32)     // Core 3 Mailbox 0 Read & Write-high-to-clear
#define CORE3_MBOX1_RW          MEM(0x400000F4, 32)     // Core 3 Mailbox 1 Read & Write-high-to-clear
#define CORE3_MBOX2_RW          MEM(0x400000F8, 32)     // Core 3 Mailbox 2 Read & Write-high-to-clear
#define CORE3_MBOX3_RW          MEM(0x400000FC, 32)     // Core 3 Mailbox 3 Read & Write-high-to-clear
