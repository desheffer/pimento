#pragma once

#define PSR_MODE_INIT    0x3C4
#define PSR_MODE_KTHREAD 0x304
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

#define STACK_TOP 0x40000000

#ifndef __ASSEMBLY__

#include <list.h>
#include <stddef.h>

typedef struct {
    long unsigned regs[31];
    long unsigned :64;
    long unsigned pstate;
    long unsigned pc;
} process_regs_t;

typedef enum {
    created,
    running,
    sleeping,
    stopping,
} process_state_t;

typedef struct {
    short unsigned pid;
    process_state_t state;
    char pname[PNAME_LENGTH];
    list_t* pages;
    long unsigned ttbr;
    void* sp;
} process_t;

#endif
