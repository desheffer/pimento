#include <string.h>
#include <system.h>

SYSCALL_DEFINE1(uname, struct utsname *, buf)
{
    strcpy(buf->sysname, "PI-OS");
    strcpy(buf->nodename, "pi");
    strcpy(buf->release, "dev");
    strcpy(buf->version, __DATE__ " " __TIME__);
    strcpy(buf->machine, "aarch64");

    return 0;
}
