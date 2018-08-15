#include <assert.h>
#include <string.h>

void test_string()
{
    char str[1024];

    assert(memcpy(str, "memcpy", 7) == str);
    assert(strcmp(str, "memcpy") == 0);

    assert(memset(str, 0, 1024) == str);
    assert(memset(str, '*', 4) == str);
    assert(strcmp(str, "****") == 0);

    assert(strcmp("abc", "abc") == 0);
    assert(strcmp("abc", "abcd") == -1);
    assert(strcmp("abcd", "abc") == 1);
    assert(strcmp("abcd", "abcz") == -1);
    assert(strcmp("abcz", "abcd") == 1);

    assert(strlen("abcd") == 4);
    assert(strlen("") == 0);

    assert(strcpy(str, "strcpy") == str);
    assert(strcmp(str, "strcpy") == 0);

    assert(strncpy(str, "", 1024) == str);
    assert(strcmp(str, "") == 0);
    assert(strncpy(str, "strncpy", 4) == str);
    assert(strcmp(str, "strn") == 0);
}
