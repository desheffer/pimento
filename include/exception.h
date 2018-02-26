#include <scheduler.h>
#include <stdint.h>

#ifndef EXCEPTION_H
#define EXCEPTION_H

#define breakpoint asm volatile("brk #0");

struct debug_state_t {
    volatile uint64_t spsr;
    volatile uint64_t elr;
    volatile uint64_t esr;
    volatile uint64_t far;
    volatile uint64_t sctlr;
    volatile uint64_t tcr;
    volatile uint64_t index;
    volatile uint64_t x[NUM_REGS];
};

extern "C" debug_state_t* debug_state;

extern "C" void exception_handler();

#endif
