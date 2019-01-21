#include <assert.h>
#include <string.h>

void test_string(void)
{
    char str[1024];
    char * saveptr = 0;

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

    assert(strncmp("abc", "abc", 3) == 0);
    assert(strncmp("abc", "abcd", 3) == 0);
    assert(strncmp("abcd", "abc", 3) == 0);
    assert(strncmp("abcd", "abcz", 3) == 0);
    assert(strncmp("abcz", "abcd", 3) == 0);
    assert(strncmp("abc", "abcd", 4) == -1);
    assert(strncmp("abcd", "abc", 4) == 1);
    assert(strncmp("abcd", "abcz", 4) == -1);
    assert(strncmp("abcz", "abcd", 4) == 1);

    assert(strlen("abcd") == 4);
    assert(strlen("") == 0);

    assert(strcpy(str, "strcpy") == str);
    assert(strcmp(str, "strcpy") == 0);

    assert(strncpy(str, "", 1024) == str);
    assert(strcmp(str, "") == 0);
    assert(strncpy(str, "strncpy", 4) == str);
    assert(strcmp(str, "strn") == 0);

    strcpy(str, "/foo/bar//baz/");
    assert(strcmp("foo", strtok_r(str, "/", &saveptr)) == 0);
    assert(strcmp("bar", strtok_r(0, "/", &saveptr)) == 0);
    assert(strcmp("baz", strtok_r(0, "/", &saveptr)) == 0);
    assert(strtok_r(0, "/", &saveptr) == 0);
}
