#pragma once

#include <interrupt.h>

#define HZ 100

void timer_init();
void timer_connect(interrupt_handler_t*, void*);
void timer_disconnect();
void timer_reset();
long unsigned timer_counter();
