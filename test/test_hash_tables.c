#include "../lib/base.h"

HASH_TABLE_DECLARE(HashTable, hash_table, const char *, i32)
HASH_TABLE_IMPLEMENT(HashTable, hash_table, const char *, i32)

TEST(hash_table_new) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    TEST_ASSERT(table.buckets != NULL);
    TEST_ASSERT(table.bucket_count == 8);
    TEST_ASSERT(table.size == 0);
    TEST_ASSERT(table.hash == cstr_hash);
    TEST_ASSERT(table.eq == cstr_eq);
    TEST_ASSERT(table.allocator == &heap_allocator);

    hash_table_free(&table);
}

TEST(hash_table_set_get) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    hash_table_set(&table, "foo", 42);
    TEST_ASSERT(hash_table_get(&table, "foo") == 42);

    hash_table_set(&table, "bar", 69);
    TEST_ASSERT(hash_table_get(&table, "bar") == 69);

    hash_table_free(&table);
}

TEST(hash_table_set_get_remove) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    hash_table_set(&table, "foo", 42);
    TEST_ASSERT(hash_table_get(&table, "foo") == 42);

    hash_table_set(&table, "bar", 69);
    TEST_ASSERT(hash_table_get(&table, "bar") == 69);

    TEST_ASSERT(hash_table_remove(&table, "foo"));
    TEST_ASSERT(hash_table_remove(&table, "bar"));
    TEST_ASSERT(!hash_table_remove(&table, "foo"));
    TEST_ASSERT(!hash_table_remove(&table, "bar"));

    hash_table_free(&table);
}

TEST(hash_table_grow) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    for (usize i = 0; i < 10; i++) {
        char buffer[6];
        snprintf(buffer, sizeof(buffer), "foo%zu", i);
        hash_table_set(&table, buffer, i);
    }

    TEST_ASSERT(table.bucket_count == 16);
    TEST_ASSERT(table.size == 10);

    hash_table_free(&table);
}

TEST(hash_table_reset) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    hash_table_set(&table, "foo", 42);
    TEST_ASSERT(hash_table_get(&table, "foo") == 42);

    hash_table_reset(&table);
    TEST_ASSERT(!hash_table_contains(&table, "foo"));

    hash_table_free(&table);
}

TEST(hash_table_contains) {
    HashTable table = hash_table_new(cstr_hash, cstr_eq, &heap_allocator);

    hash_table_set(&table, "foo", 42);
    TEST_ASSERT(hash_table_contains(&table, "foo"));
    TEST_ASSERT(!hash_table_contains(&table, "bar"));

    hash_table_free(&table);
}

void test_suite_hash_table(void) {
    TEST_RUN(hash_table_new);
    TEST_RUN(hash_table_set_get);
    TEST_RUN(hash_table_set_get_remove);
    TEST_RUN(hash_table_grow);
    TEST_RUN(hash_table_reset);
    TEST_RUN(hash_table_contains);
}
