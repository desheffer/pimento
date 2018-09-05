#pragma once

#include <bcm2837.h>
#include <process.h>

#define NUM_IRQS 10

typedef enum {
    local_irq_cntpsirq  = 0,
    local_irq_cntpnsirq = 1,
    local_irq_cnthpirq  = 2,
    local_irq_cntvirq   = 3,
    local_irq_mailbox0  = 4,
    local_irq_mailbox1  = 5,
    local_irq_mailbox2  = 6,
    local_irq_mailbox3  = 7,
    local_irq_gpu_fast  = 8,
    local_irq_pmu_fast  = 9,
} irq_number_t;

typedef void interrupt_handler_t(void*);

void interrupt_init();
void interrupt_connect(irq_number_t, interrupt_handler_t*, void*);
void interrupt_disconnect(irq_number_t);
process_regs_t* interrupt_handler(process_regs_t* state);

void disable_interrupts();
void enable_interrupts();
