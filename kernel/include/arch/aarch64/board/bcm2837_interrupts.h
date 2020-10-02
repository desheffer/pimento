#pragma once

#include "asm/entry.h"
#include "pimento.h"

#define BCM2837_NUM_IRQS 10

#define BCM2837_CORE_TIMERS_CONTROL   ((volatile uint32_t *) (0xFFFF000040000000)) // Control Register
#define BCM2837_CORE_TIMERS_PRESCALER ((volatile uint32_t *) (0xFFFF000040000008)) // Core Timer Prescaler
#define BCM2837_CORE0_TIMERS_CNTL     ((volatile uint32_t *) (0xFFFF000040000040)) // Core 0 Timers Interrupt control
#define BCM2837_CORE0_IRQ             ((volatile uint32_t *) (0xFFFF000040000060)) // Core 0 IRQ Source

#define BCM2837_LOCAL_IRQ_CNTPSIRQ  0
#define BCM2837_LOCAL_IRQ_CNTPNSIRQ 1
#define BCM2837_LOCAL_IRQ_CNTHPIRQ  2
#define BCM2837_LOCAL_IRQ_CNTVIRQ   3
#define BCM2837_LOCAL_IRQ_MAILBOX0  4
#define BCM2837_LOCAL_IRQ_MAILBOX1  5
#define BCM2837_LOCAL_IRQ_MAILBOX2  6
#define BCM2837_LOCAL_IRQ_MAILBOX3  7
#define BCM2837_LOCAL_IRQ_GPU_FAST  8
#define BCM2837_LOCAL_IRQ_PMU_FAST  9

void bcm2837_interrupts_init(void);
void bcm2837_interrupts_connect(unsigned, interrupt_handler_t, void *);
void bcm2837_interrupts_disconnect(unsigned);
void bcm2837_interrupts_handler(void);
