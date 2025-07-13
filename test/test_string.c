#include "../lib/base.h"

TEST(string_from_cstr) {
    String str = string("Hello, world!", &heap_allocator);

    TEST_ASSERT(str.buffer != NULL);
    TEST_ASSERT(str.length == 13);
    TEST_ASSERT(string_eq_cstr(str, "Hello, world!"));

    string_free(&str);
}

TEST(string_concat) {
    Arena scratch = arena_new(128, &heap_allocator);
    String a = string("Hello, ", &scratch.allocator);
    String b = string("world!", &scratch.allocator);
    String c = string_concat(a, b, &heap_allocator);

    arena_free(&scratch);

    TEST_ASSERT(c.buffer != NULL);
    TEST_ASSERT(c.length == 13);
    TEST_ASSERT(string_eq_cstr(c, "Hello, world!"));

    string_free(&c);
}

TEST(string_slice) {
    String str = string("Hello, world!", &heap_allocator);

    String slice = string_slice(str, 7, str.length - 1);

    TEST_ASSERT(slice.buffer != NULL);
    TEST_ASSERT(slice.length == 5);
    TEST_ASSERT(string_eq_cstr(slice, "world"));

    string_free(&str);
    string_free(&slice);
}

TEST(string_eq) {
    String a = string("Hello, world!", &heap_allocator);
    String b = string("Hello, world!", &heap_allocator);

    TEST_ASSERT(string_eq(a, b));

    string_free(&a);
    string_free(&b);
}

TEST(string_not_eq) {
    String a = string("Hello, world!", &heap_allocator);
    String b = string("Hello, world?", &heap_allocator);

    TEST_ASSERT(!string_eq(a, b));

    string_free(&a);
    string_free(&b);
}

TEST(string_not_eq_cstr) {
    String a = string("Hello, world!", &heap_allocator);

    TEST_ASSERT(!string_eq_cstr(a, "Hello, world?"));

    string_free(&a);
}

void test_suite_string(void) {
    TEST_RUN(string_from_cstr);
    TEST_RUN(string_concat);
    TEST_RUN(string_slice);
    TEST_RUN(string_eq);
    TEST_RUN(string_not_eq);
    TEST_RUN(string_not_eq_cstr);
}
