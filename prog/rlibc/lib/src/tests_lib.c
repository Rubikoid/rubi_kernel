#include <kernel/vga/vga.h>
#include <lib/stdio.h>
#include <lib/string.h>
#include <types.h>

#define __MODULE_NAME__ "LIBC_TEST"

void test_strcmp() {
    assert(strcmp("aa", "aa") == 0);

    assert(strcmp("ab", "aa") == 1);
    assert(strcmp("ac", "aa") == 2);

    assert(strcmp("aa", "ab") == -1);
    assert(strcmp("aa", "ac") == -2);

    assert(strcmp("aa", "a") == 97);
    assert(strcmp("a", "aa") == -97);

    ulog("strcmp test [" G_GREEN "OK" G_WHITE "]\n");
}
