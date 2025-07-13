#include "../lib/base.h"

TEST(heap_allocator_alloc) {
    i32 *ptr = (i32 *)heap_allocator.alloc(&heap_allocator, sizeof(i32));
    *ptr = 42;

    TEST_ASSERT(*ptr == 42);

    heap_allocator.free(&heap_allocator, ptr);
}

TEST(heap_allocator_zero) {
    i32 *ptr = (i32 *)heap_allocator.alloc(&heap_allocator, sizeof(i32));

    TEST_ASSERT(*ptr == 0);

    heap_allocator.free(&heap_allocator, ptr);
}

TEST(heap_allocator_realloc) {
    i32 *ptr = (i32 *)heap_allocator.alloc(&heap_allocator, sizeof(i32));
    *ptr = 42;

    i64 *ptr2 = (i64 *)heap_allocator.realloc(&heap_allocator, ptr, sizeof(i32), sizeof(i64));

    TEST_ASSERT((i32)*ptr2 == 42);

    ptr = (i32 *)heap_allocator.realloc(&heap_allocator, ptr, sizeof(i64), sizeof(i32));
    *ptr = 420;

    TEST_ASSERT(*ptr == 420);

    heap_allocator.free(&heap_allocator, ptr);
}

TEST(arena_new) {
    Arena arena = arena_new(8, &heap_allocator);

    TEST_ASSERT(arena.allocator.alloc != NULL);
    TEST_ASSERT(arena.allocator.free != NULL);
    TEST_ASSERT(arena.buffer != NULL);
    TEST_ASSERT(arena.capacity == 8);
    TEST_ASSERT(arena.offset == 0);

    arena_free(&arena);
}

TEST(arena_alloc) {
    Arena arena = arena_new(8, &heap_allocator);
    i32 *ptr = (i32 *)arena_alloc(&arena, sizeof(i32));

    TEST_ASSERT(*ptr == 0);

    *ptr = 42;

    TEST_ASSERT(arena.buffer != NULL);
    TEST_ASSERT(arena.capacity == 8);
    TEST_ASSERT(arena.offset == sizeof(i32));
    TEST_ASSERT(*ptr == 42);

    arena_free(&arena);
}

TEST(arena_realloc) {
    Arena arena = arena_new(8, &heap_allocator);
    i32 *ptr = (i32 *)arena_alloc(&arena, sizeof(i32));
    *ptr = 42;

    i64 *ptr2 = (i64 *)arena_realloc(&arena, ptr, sizeof(i32), sizeof(i64));

    TEST_ASSERT((i32)*ptr2 == 42);

    ptr = (i32 *)arena_realloc(&arena, ptr, sizeof(i64), sizeof(i32));
    *ptr = 420;

    TEST_ASSERT(*ptr == 420);

    arena_free(&arena);
}

TEST(arena_reset) {
    Arena arena = arena_new(8, &heap_allocator);
    i32 *ptr = (i32 *)arena_alloc(&arena, sizeof(i32));
    *ptr = 42;

    arena_reset(&arena);

    TEST_ASSERT(arena.buffer != NULL);
    TEST_ASSERT(arena.capacity == 8);
    TEST_ASSERT(arena.offset == 0);

    ptr = (i32 *)arena_alloc(&arena, sizeof(i32));
    *ptr = 69;

    TEST_ASSERT(arena.buffer != NULL);
    TEST_ASSERT(arena.capacity == 8);
    TEST_ASSERT(arena.offset == sizeof(i32));
    TEST_ASSERT(*ptr == 69);

    arena_free(&arena);
}

TEST(arena_allocator_alloc) {
    Arena arena = arena_new(8, &heap_allocator);
    Allocator *allocator = &arena.allocator;
    i32 *ptr = (i32 *)allocator->alloc(allocator, sizeof(i32));
    *ptr = 42;

    i32 *ptr2 = (i32 *)allocator->alloc(allocator, sizeof(i32));
    *ptr2 = 69;

    TEST_ASSERT(*ptr == 42);
    TEST_ASSERT(*ptr2 == 69);

    allocator->free(allocator, ptr);
    allocator->free(allocator, ptr2);

    TEST_ASSERT(*ptr == 42);
    TEST_ASSERT(*ptr2 == 69);

    arena_reset(&arena);

    i64 *ptr3 = (i64 *)arena_alloc(&arena, sizeof(i64));
    *ptr3 = 420;

    TEST_ASSERT(*ptr == 420);

    arena_free(&arena);
}

TEST(arena_nested) {
    Arena arena = arena_new(1024, &heap_allocator);
    Arena arena2 = arena_new(8, &arena.allocator);
    i32 *ptr = (i32 *)arena_alloc(&arena2, sizeof(i32));
    *ptr = 42;

    TEST_ASSERT(arena.buffer != NULL);
    TEST_ASSERT(arena.capacity == 1024);
    TEST_ASSERT(arena.offset == 8);

    TEST_ASSERT(arena2.buffer != NULL);
    TEST_ASSERT(arena2.capacity == 8);
    TEST_ASSERT(arena2.offset == 4);

    TEST_ASSERT(*ptr == 42);

    arena_free(&arena);
}

void test_suite_heap_allocator(void) {
    TEST_RUN(heap_allocator_alloc);
    TEST_RUN(heap_allocator_zero);
    TEST_RUN(heap_allocator_realloc);
}

void test_suite_arena(void) {
    TEST_RUN(arena_new);
    TEST_RUN(arena_alloc);
    TEST_RUN(arena_realloc);
    TEST_RUN(arena_reset);
    TEST_RUN(arena_allocator_alloc);
    TEST_RUN(arena_nested);
}
