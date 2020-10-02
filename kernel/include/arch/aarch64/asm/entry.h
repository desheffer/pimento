#pragma once

#define PROCESS_REGS_SIZE (34 * 8)

#define BAD_SYNC  0
#define BAD_IRQ   1
#define BAD_FIQ   2
#define BAD_ERROR 3

#define DAIF_D (0b1 << 3)
#define DAIF_A (0b1 << 2)
#define DAIF_I (0b1 << 1)
#define DAIF_F (0b1 << 0)

#define ESR_ELx_EC_SHIFT 26
#define ESR_ELx_EC_MASK  (0b111111UL << ESR_ELx_EC_SHIFT)

#define ESR_ELx_EC_SVC64    0b010101
#define ESR_ELx_EC_DABT_LOW 0b100100

#ifndef __ASSEMBLER__

#include "pimento.h"

typedef void (* interrupts_handler_t)(void);
typedef void (* interrupt_handler_t)(void *);

struct registers {
    uint64_t x[31]; // x0 - x30
    uint64_t sp;
    uint64_t pc;
    uint64_t pstate;
} __attribute__((packed));

void interrupts_handler(void);
void timer_set(void);
void set_interrupts_handler(interrupts_handler_t);

void load_regs(void);

#endif
