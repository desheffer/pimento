#include <assert.h>
#include <kstdio.h>
#include <string.h>

void test_printf(void)
{
    char buf[1024];
    unsigned len;

    len = sprintf(buf, "hello");
    assert(strcmp("hello", buf) == 0);
    assert(len == 5);

    sprintf(buf, "%%");
    assert(strcmp("%", buf) == 0);

    sprintf(buf, "hello %s", "world");
    assert(strcmp("hello world", buf) == 0);

    sprintf(buf, "%u %d %d", 123, 123, -123);
    assert(strcmp("123 123 -123", buf) == 0);

    sprintf(buf, "%x %X %o", 0xABC, 0xABC, 83);
    assert(strcmp("abc ABC 123", buf) == 0);

    sprintf(buf, "%u %d %x %X %o", 0, 0, 0, 0, 0);
    assert(strcmp("0 0 0 0 0", buf) == 0);

    sprintf(buf, "%#x %#X %#o", 0xABC, 0xABC, 83);
    assert(strcmp("0xabc 0XABC 0123", buf) == 0);

    sprintf(buf, "%8s %8c", "123", 'c');
    assert(strcmp("     123        c", buf) == 0);

    sprintf(buf, "%8u %8d %8d %8x %8o", 123, 123, -123, 0x123, 83);
    assert(strcmp("     123      123     -123      123      123", buf) == 0);

    sprintf(buf, "%-8s %-8c", "123", 'c');
    assert(strcmp("123      c       ", buf) == 0);

    sprintf(buf, "%-8u %-8d %-8d %-8x %-8o", 123, 123, -123, 0x123, 83);
    assert(strcmp("123      123      -123     123      123     ", buf) == 0);

    sprintf(buf, "%08u %08d %08d %08x %08o", 123, 123, -123, 0x123, 83);
    assert(strcmp("00000123 00000123 -0000123 00000123 00000123", buf) == 0);

    sprintf(buf, "%#010x %#010X %#010o", 0xabc, 0xABC, 83);
    assert(strcmp("0x00000abc 0X00000ABC 0000000123", buf) == 0);

    sprintf(buf, "%+d %+d", 123, -123);
    assert(strcmp("+123 -123", buf) == 0);

    sprintf(buf, "%*d", 8, 123);
    assert(strcmp("     123", buf) == 0);
}
