#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef struct {
    int fd;
} FILE;

typedef void (*putc_t)(void*, char);

#ifdef __cplusplus
extern "C" {
#endif
    void cprintf(void*, putc_t, const char*, ...) __attribute__((format(printf, 3, 4)));
    char* fgets(char*, int, FILE*);
    void printf(const char*, ...) __attribute__((format(printf, 1, 2)));
    size_t snprintf(char*, size_t, const char*, ...) __attribute__((format(printf, 3, 4)));
    size_t sprintf(char*, const char*, ...) __attribute__((format(printf, 2, 3)));
    void vcprintf(void*, putc_t, const char*, va_list);
    void vprintf(const char*, va_list);
    size_t vsnprintf(char*, size_t, const char*, va_list);
    size_t vsprintf(char*, const char*, va_list);
#ifdef __cplusplus
}
#endif

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

// @TODO: Provide proper constants.
#define stdin ((FILE*) 0)
#define stdout ((FILE*) 0)
#define stderr ((FILE*) 0)
