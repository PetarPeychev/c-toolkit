#include "../lib/base.h"

DYNAMIC_ARRAY_DECLARE(DA, array, i32)
DYNAMIC_ARRAY_IMPLEMENT(DA, array, i32)

TEST(dynamic_array_new) {
    DA array = array_new(&heap_allocator);

    TEST_ASSERT(array.data != NULL);
    TEST_ASSERT(array.length == 0);
    TEST_ASSERT(array.capacity == 8);
    TEST_ASSERT(array.allocator == &heap_allocator);

    array_free(&array);
}

TEST(dynamic_array_push) {
    DA array = array_new(&heap_allocator);

    array_push(&array, 42);

    TEST_ASSERT(array.data != NULL);
    TEST_ASSERT(array.length == 1);
    TEST_ASSERT(array.capacity == 8);
    TEST_ASSERT(array.allocator == &heap_allocator);

    TEST_ASSERT(array.data[0] == 42);

    array_free(&array);
}

TEST(dynamic_array_push_grow) {
    DA array = array_new(&heap_allocator);

    for (usize i = 0; i < 10; i++) {
        array_push(&array, i);
    }

    TEST_ASSERT(array.data != NULL);
    TEST_ASSERT(array.length == 10);
    TEST_ASSERT(array.capacity == 16);
    TEST_ASSERT(array.allocator == &heap_allocator);

    for (usize i = 0; i < 10; i++) {
        TEST_ASSERT(array.data[i] == i);
    }

    array_free(&array);
}

TEST(dynamic_array_push_pop) {
    DA array = array_new(&heap_allocator);
    array_push(&array, 42);
    array_push(&array, 69);
    i32 value = array_pop(&array);

    TEST_ASSERT(array.data != NULL);
    TEST_ASSERT(array.length == 1);
    TEST_ASSERT(array.capacity == 8);
    TEST_ASSERT(array.allocator == &heap_allocator);

    TEST_ASSERT(value == 69);
    TEST_ASSERT(array.data[0] == 42);

    array_free(&array);
}

TEST(dynamic_array_extend) {
    DA array1 = array_new(&heap_allocator);
    DA array2 = array_new(&heap_allocator);

    for (usize i = 0; i < 3; i++) {
        array_push(&array1, i);
    }
    for (usize i = 3; i < 6; i++) {
        array_push(&array2, i);
    }

    array_extend(&array1, &array2);

    TEST_ASSERT(array1.data != NULL);
    TEST_ASSERT(array1.length == 6);
    TEST_ASSERT(array1.capacity == 8);
    TEST_ASSERT(array1.allocator == &heap_allocator);

    for (usize i = 0; i < 6; i++) {
        TEST_ASSERT(array1.data[i] == i);
    }

    array_free(&array1);
    array_free(&array2);
}

TEST(dynamic_array_slice) {
    DA array = array_new(&heap_allocator);

    for (usize i = 0; i < 10; i++) {
        array_push(&array, i);
    }

    DA slice = array_slice(&array, 3, 6);

    TEST_ASSERT(slice.data != NULL);
    TEST_ASSERT(slice.length == 3);
    TEST_ASSERT(slice.capacity == 0);
    TEST_ASSERT(slice.allocator == NULL);

    for (usize i = 0; i < 3; i++) {
        TEST_ASSERT(slice.data[i] == i + 3);
    }

    array_free(&array);
}

void test_suite_dynamic_array(void) {
    TEST_RUN(dynamic_array_new);
    TEST_RUN(dynamic_array_push);
    TEST_RUN(dynamic_array_push_grow);
    TEST_RUN(dynamic_array_push_pop);
    TEST_RUN(dynamic_array_extend);
    TEST_RUN(dynamic_array_slice);
}
