#pragma once

#include <process.h>

process_t* process_create(const char*, const void*, const void*);
void move_to_user_mode(void*, void*);
