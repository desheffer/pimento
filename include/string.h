#pragma once

#include <stddef.h>
#include <stdint.h>

void* memcpy(void*, const void*, size_t);
void* memset(void*, uint8_t, size_t);
int32_t strcmp(const char*, const char*);
size_t strlen(const char*);
