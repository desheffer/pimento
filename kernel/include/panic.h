#pragma once

#include <asm/entry.h>

void debug_process_regs(struct registers *, long unsigned, long unsigned, long unsigned);
void halt(void);
void panic(void);
