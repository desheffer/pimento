#pragma once

#define NUM_IRQS 10

enum irq_number {
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
};

typedef void interrupt_handler_t(void*);

void interrupt_init(void);
void interrupt_connect(enum irq_number, interrupt_handler_t*, void*);
void interrupt_disconnect(enum irq_number);
void interrupt_handler(void);

void disable_interrupts(void);
void enable_interrupts(void);
