#pragma once

#include <scheduler.h>
#include <stdint.h>

extern "C" void exception_handler(process_state_t*, uint64_t, uint64_t, uint64_t);
extern "C" void debug(process_state_t*, uint64_t, uint64_t, uint64_t);
