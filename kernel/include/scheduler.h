#pragma once

#include <process.h>

void scheduler_init();
void scheduler_add(process_control_block_t*);
unsigned scheduler_current_pid();
process_regs_t* scheduler_next(process_regs_t*);
void scheduler_start();
void scheduler_tick();
