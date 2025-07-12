#include "test_allocators.c"
#include "test_strings.c"

#define BASE_IMPLEMENTATION
#include "../lib/base.h"

#define TEST_IMPLEMENTATION
#include "../lib/test.h"

int main(void) {
    test_suite_heap_allocator();
    test_suite_arena();
    test_suite_string();

    return TEST_RESULTS();
}
