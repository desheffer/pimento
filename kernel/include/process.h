#pragma once

#define PSR_MODE_INIT    0x3C4
#define PSR_MODE_KTHREAD 0x304
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

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
    process_regs_t* regs;
    long unsigned ttbr;
    list_t* pages;
} process_t;

void process_init();
process_regs_t* process_context_switch(process_regs_t*);
unsigned process_count();
process_t* process_create(const char*, const void*, const void*);
process_t* process_current();
void process_destroy(process_t*);

#endif
