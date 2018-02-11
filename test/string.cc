#include <assert.h>
#include <string.h>

void test_string()
{
    assert(strcmp("abc", "abc") == 0);
    assert(strcmp("abc", "abcd") == -1);
    assert(strcmp("abcd", "abc") == 1);
    assert(strcmp("abcd", "abcz") == -1);
    assert(strcmp("abcz", "abcd") == 1);

    assert(strlen("abcd") == 4);
    assert(strlen("") == 0);
}
