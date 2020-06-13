#pragma once

#define MAP_ARG0(...)
#define MAP_ARG1(map, vtype, vname, ...) map(vtype, vname)
#define MAP_ARG2(map, vtype, vname, ...) map(vtype, vname), MAP_ARG1(map, __VA_ARGS__)
#define MAP_ARG3(map, vtype, vname, ...) map(vtype, vname), MAP_ARG2(map, __VA_ARGS__)
#define MAP_ARG4(map, vtype, vname, ...) map(vtype, vname), MAP_ARG3(map, __VA_ARGS__)
#define MAP_ARG5(map, vtype, vname, ...) map(vtype, vname), MAP_ARG4(map, __VA_ARGS__)
#define MAP_ARG6(map, vtype, vname, ...) map(vtype, vname), MAP_ARG5(map, __VA_ARGS__)
#define MAP_ARGn(num, map, ...)          MAP_ARG ## num(map, __VA_ARGS__)

#define MAP_TYPE_NAME(vtype, vname) vtype vname
#define MAP_NAME(vtype, vname)      vname

#define SYSCALL_DEFINEn(num, name, ...) \
    static long _abi_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__)); \
    long abi_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__)) \
    { \
        long ret = _abi_ ## name(MAP_ARGn(num, MAP_NAME, __VA_ARGS__)); \
        return ret; \
    } \
    long _abi_ ## name(MAP_ARGn(num, MAP_TYPE_NAME, __VA_ARGS__))

#define SYSCALL_DEFINE0(name, ...) SYSCALL_DEFINEn(0, name, __VA_ARGS__)
#define SYSCALL_DEFINE1(name, ...) SYSCALL_DEFINEn(1, name, __VA_ARGS__)
#define SYSCALL_DEFINE2(name, ...) SYSCALL_DEFINEn(2, name, __VA_ARGS__)
#define SYSCALL_DEFINE3(name, ...) SYSCALL_DEFINEn(3, name, __VA_ARGS__)
#define SYSCALL_DEFINE4(name, ...) SYSCALL_DEFINEn(4, name, __VA_ARGS__)
#define SYSCALL_DEFINE5(name, ...) SYSCALL_DEFINEn(5, name, __VA_ARGS__)
#define SYSCALL_DEFINE6(name, ...) SYSCALL_DEFINEn(6, name, __VA_ARGS__)

typedef long (* syscall_t)();

long abi_invalid(int);
