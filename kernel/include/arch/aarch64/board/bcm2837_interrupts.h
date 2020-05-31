#pragma once

#include <asm/entry.h>
#include <stdint.h>

#define NUM_IRQS 10

#define CORE_TIMERS_CONTROL   ((volatile uint32_t *) (0x40000000)) // Control Register
#define CORE_TIMERS_PRESCALER ((volatile uint32_t *) (0x40000008)) // Core Timer Prescaler
#define CORE0_TIMERS_CNTL     ((volatile uint32_t *) (0x40000040)) // Core 0 Timers Interrupt control
#define CORE0_IRQ             ((volatile uint32_t *) (0x40000060)) // Core 0 IRQ Source

#define LOCAL_IRQ_CNTPSIRQ  0
#define LOCAL_IRQ_CNTPNSIRQ 1
#define LOCAL_IRQ_CNTHPIRQ  2
#define LOCAL_IRQ_CNTVIRQ   3
#define LOCAL_IRQ_MAILBOX0  4
#define LOCAL_IRQ_MAILBOX1  5
#define LOCAL_IRQ_MAILBOX2  6
#define LOCAL_IRQ_MAILBOX3  7
#define LOCAL_IRQ_GPU_FAST  8
#define LOCAL_IRQ_PMU_FAST  9

void bcm2837_interrupts_init(void);
void bcm2837_interrupts_connect(unsigned, interrupt_handler_t, void *);
void bcm2837_interrupts_disconnect(unsigned);
void bcm2837_interrupts_handler(void);
