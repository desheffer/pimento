#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    int execve(const char*, char* const[], char* const[]);
#ifdef __cplusplus
}
#endif
