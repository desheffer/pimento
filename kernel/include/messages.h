#pragma once

#define RESET "\e[0m"

#define BG_RED  "\e[41m\e[97m"
#define BG_BLUE "\e[44m\e[97m"

#define FG_RED  "\e[31m"

#define WELCOME_MSG \
    "\n" \
    BG_BLUE "                 " RESET "\n" \
    BG_BLUE "     Pimento     " RESET "\n" \
    BG_BLUE "                 " RESET "\n" \
    "\n"

#define EXCEPTION_MSG \
    "\n\n" \
    BG_RED "                          " RESET "\n" \
    BG_RED "     Kernel Exception     " RESET "\n" \
    BG_RED "                          " RESET "\n" \
    "\n"
