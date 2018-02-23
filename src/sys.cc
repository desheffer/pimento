#include <stdio.h>
#include <sys.h>

void halt()
{
    asm volatile("msr daifset, #2");

    while (1) {
        asm volatile("wfi");
    }
}

void panic()
{
    printf("\n\n!!! Kernel Panic! !!!\n");
    halt();
}

void exception_handler(uint64_t index, uint64_t esr)
{
    printf("\n\n!!! Kernel Exception! !!!\n");

    const char* type = "Unknown";
    const char* ifs = "Unknown";
    const char* dfs = "Unknown";
    const char* level = "Unknown";

    switch (index) {
        case 0: type = "Synchronous"; break;
        case 1: type = "IRQ"; break;
        case 2: type = "FIQ"; break;
        case 3: type = "SError"; break;
    }

    switch (esr >> 26) {
        case 0b000000: ifs = "Unknown reason"; break;
        case 0b000001: ifs = "Trapped WFI or WFE instruction"; break;
        case 0b001110: ifs = "Illegal Execution state"; break;
        case 0b010101: ifs = "SVC instruction"; break;
        case 0b100000: ifs = "Instruction Abort, lower level"; break;
        case 0b100001: ifs = "Instruction Abort, same level"; break;
        case 0b100010: ifs = "PC alignment fault"; break;
        case 0b100100: ifs = "Data Abort, lower level"; break;
        case 0b100101: ifs = "Data Abort, same level"; break;
        case 0b100110: ifs = "SP alignment fault"; break;
        case 0b101100: ifs = "Trapped floating-point"; break;
    }

    if (esr >> 26 == 0b100100 || esr >> 26 == 0b100101) {
        switch (esr & 0b01100) {
            case 0b00000: dfs = "Address size fault"; break;
            case 0b00100: dfs = "Translation fault"; break;
            case 0b01000: dfs = "Access flag fault"; break;
            case 0b01100: dfs = "Permission fault"; break;
        }

        switch (esr & 0b11) {
            case 0b00: level = "0"; break;
            case 0b01: level = "1"; break;
            case 0b10: level = "2"; break;
            case 0b11: level = "3"; break;
        }
    }

    printf(
        "Exception Type = %s\n"
        "Instruction Fault Status = %s\n"
        "Data Fault Status = %s, level %s\n",
        type, ifs, dfs, level
    );

    halt();
}
