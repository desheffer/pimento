#include <stddef.h>
#include <stdint.h>

#ifndef STRING_H
#define STRING_H

void* memcpy(void*, const void*, uint32_t);
void* memset(void*, uint8_t, uint32_t);
int32_t strcmp(const char*, const char*);
size_t strlen(const char*);

#endif
