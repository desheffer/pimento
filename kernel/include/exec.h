#pragma once

struct binprm {
    char * filename;
    char ** argv;
    char ** envp;
};

int process_exec(const char *, char * const *, char * const *);
