#pragma once

#include <process.h>

void scheduler_init();
void scheduler_create_process(const char*, const void*);
unsigned scheduler_process_count();
process_regs_t* scheduler_schedule(process_regs_t*);
void scheduler_stop_process();
void scheduler_tick();
