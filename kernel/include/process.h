#pragma once

#define PSR_MODE_INIT    0x3C5
#define PSR_MODE_KTHREAD 0x305
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

#define STACK_TOP 0x40000000

#ifndef __ASSEMBLY__

#include <entry.h>
#include <list.h>
#include <stddef.h>

typedef enum {
    created,
    running,
    sleeping,
    stopping,
} process_state_t;

typedef struct {
    long unsigned regs[11]; // x19 - x29
    long unsigned sp;
    long unsigned pc;
} cpu_context_t;

typedef struct {
    short unsigned pid;
    process_state_t state;
    char pname[PNAME_LENGTH];
    cpu_context_t* cpu_context;
    list_t* pages;
    long unsigned ttbr;
} process_t;

process_t* process_create_kernel();
void process_destroy(process_t*);
void* process_set_args(void*, char* const[], char* const[]);

#endif
