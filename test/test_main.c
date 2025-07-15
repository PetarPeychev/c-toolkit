#define TESTS_ENABLED

#include "test_allocators.c"
#include "test_string.c"
#include "test_dynamic_array.c"
#include "test_hash_tables.c"

#define BASE_IMPLEMENTATION
#include "../lib/base.h"

int main(void) {
    test_suite_heap_allocator();
    test_suite_arena();
    test_suite_string();
    test_suite_dynamic_array();
    test_suite_hash_table();

    return TEST_RESULTS();
}
