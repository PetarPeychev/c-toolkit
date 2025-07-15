#ifndef BASE_DECLARATIONS
#define BASE_DECLARATIONS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>

// -------------------
// --- Basic Types ---
// -------------------

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef float f32;
typedef double f64;

typedef size_t usize;

// ------------------
// --- Assertions ---
// ------------------

#define ASSERT(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "ASSERTION FAILED: %s at %s:%d in %s()\n", \
                #condition, __FILE__, __LINE__, __func__); \
        abort(); \
    } \
} while(0)

// ---------------
// --- Testing ---
// ---------------

#define TEST(name) static void test_##name(void)
int TEST_RESULTS(void);

#ifdef TESTS_ENABLED

bool _test_failed = false;
i32 _tests_ran = 0;
i32 _tests_failed = 0;

#define TEST_ASSERT(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "\tASSERTION FAILED: %s at %s:%d in %s()\n", \
                #condition, __FILE__, __LINE__, __func__); \
        _test_failed = true; \
    } \
} while(0)

#define TEST_RUN(name) do { \
    printf("Running test %s... \n", #name); \
    _tests_ran++; \
    test_##name(); \
    if (_test_failed) { \
        _tests_failed++; \
        _test_failed = false; \
    } \
} while(0)

#else // TESTS_ENABLED

#define TEST_ASSERT(condition)
#define TEST_RUN(name)

#endif // TESTS_ENABLED

// -------------------------
// --- Memory Allocation ---
// -------------------------

typedef struct Allocator Allocator;

struct Allocator {
    void *(*alloc)(Allocator *allocator, usize size);
    void *(*realloc)(Allocator *allocator, void *ptr, usize old_size, usize new_size);
    void (*free)(Allocator *allocator, void *ptr);
};

extern Allocator heap_allocator;

typedef struct {
    Allocator allocator;
    Allocator *backing_allocator;
    u8 *buffer;
    usize capacity;
    usize offset;
    void *last_alloc;
} Arena;

Arena arena_new(usize capacity, Allocator *backing_allocator);
void *arena_alloc(Arena *arena, usize size);
void *arena_realloc(Arena *arena, void *ptr, usize old_size, usize new_size);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);

// ---------------
// --- Strings ---
// ---------------

typedef struct {
    u8 *buffer;
    usize length;
    Allocator *allocator;
} String;

String string(const char *cstr, Allocator *allocator);
String string_new(usize length, Allocator *allocator);
String string_concat(String a, String b, Allocator *allocator);
String string_slice(String string, usize start, usize end);
bool string_eq(String a, String b);
bool string_eq_cstr(String a, const char *cstr);
u64 string_hash(String str);
void string_free(String *string);

// ----------------------
// --- Dynamic Arrays ---
// ----------------------

#define DYNAMIC_ARRAY_DECLARE(name, prefix, type) \
    typedef struct { \
        type *data; \
        usize length; \
        usize capacity; \
        Allocator *allocator; \
    } name; \
    \
    name prefix##_new(Allocator *allocator); \
    void prefix##_push(name *array, type value); \
    type prefix##_pop(name *array); \
    name prefix##_extend(name *array, name *other); \
    name prefix##_slice(name *array, usize start, usize end); \
    void prefix##_reset(name *array); \
    void prefix##_free(name *array); \

#define DYNAMIC_ARRAY_IMPLEMENT(name, prefix, type) \
    name prefix##_new(Allocator *allocator) { \
        name array = { \
            .data = (type *)allocator->alloc(allocator, sizeof(type)*8), \
            .length = 0, \
            .capacity = 8, \
            .allocator = allocator \
        }; \
        return array; \
    } \
    \
    void prefix##_push(name *array, type value) { \
        if (array->length == array->capacity) { \
            array->data = (type *)array->allocator->realloc( \
                array->allocator, \
                array->data, \
                sizeof(type)*array->capacity, \
                sizeof(type)*array->capacity*2 \
            ); \
            array->capacity *= 2; \
        } \
        array->data[array->length++] = value; \
    } \
    \
    type prefix##_pop(name *array) { \
        ASSERT(array->length > 0); \
        return array->data[--array->length]; \
    } \
    \
    name prefix##_extend(name *array, name *other) { \
        if (array->length + other->length > array->capacity) { \
            array->data = (type *)array->allocator->realloc( \
                array->allocator, \
                array->data, \
                sizeof(type)*array->capacity, \
                sizeof(type)*array->capacity*2 \
            ); \
            array->capacity *= 2; \
        } \
        memcpy(array->data + array->length, other->data, sizeof(type)*other->length); \
        array->length += other->length; \
        return *array; \
    } \
    \
    name prefix##_slice(name *array, usize start, usize end) { \
        ASSERT(start <= end); \
        ASSERT(end <= array->length); \
        name slice = { \
            .data = array->data + start, \
            .length = end - start, \
            .capacity = 0, \
            .allocator = NULL \
        }; \
        return slice; \
    } \
    \
    void prefix##_reset(name *array) { \
        array->length = 0; \
    } \
    \
    void prefix##_free(name *array) { \
        if (array->allocator == NULL) return; \
        array->allocator->free(array->allocator, array->data); \
        array->data = NULL; \
        array->length = 0; \
        array->capacity = 0; \
    } \

// -------------------
// --- Hash Tables ---
// -------------------

u64 integer_hash(u64 value);
bool integer_eq(u64 a, u64 b);
u64 cstr_hash(const char *str);
bool cstr_eq(const char *a, const char *b);

#define HASH_TABLE_DECLARE(name, prefix, key_type, value_type) \
    typedef struct name##Entry { \
        key_type key; \
        value_type value; \
        struct name##Entry *next; \
    } name##Entry; \
    \
    typedef struct { \
        name##Entry **buckets; \
        usize bucket_count; \
        usize size; \
        u64 (*hash)(key_type key); \
        bool (*eq)(key_type a, key_type b); \
        Allocator *allocator; \
    } name; \
    \
    name prefix##_new(u64 (*hash)(key_type), bool (*eq)(key_type, key_type), Allocator *allocator); \
    void prefix##_set(name *table, key_type key, value_type value); \
    bool prefix##_contains(name *table, key_type key); \
    value_type prefix##_get(name *table, key_type key); \
    bool prefix##_remove(name *table, key_type key); \
    void prefix##_reset(name *table); \
    void prefix##_free(name *table); \

#define HASH_TABLE_IMPLEMENT(name, prefix, key_type, value_type) \
    static void prefix##_grow(name *table) { \
        usize old_bucket_count = table->bucket_count; \
        name##Entry **old_buckets = table->buckets; \
        \
        table->bucket_count *= 2; \
        table->buckets = (name##Entry **)table->allocator->alloc( \
            table->allocator, sizeof(name##Entry *) * table->bucket_count); \
        table->size = 0; \
        \
        for (usize i = 0; i < old_bucket_count; i++) { \
            name##Entry *entry = old_buckets[i]; \
            while (entry != NULL) { \
                name##Entry *next = entry->next; \
                \
                u64 hash = table->hash(entry->key); \
                usize bucket_index = hash % table->bucket_count; \
                \
                entry->next = table->buckets[bucket_index]; \
                table->buckets[bucket_index] = entry; \
                table->size++; \
                \
                entry = next; \
            } \
        } \
        \
        table->allocator->free(table->allocator, old_buckets); \
    } \
    \
    name prefix##_new(u64 (*hash)(key_type), bool (*eq)(key_type, key_type), Allocator *allocator) { \
        name table = { \
            .buckets = (name##Entry **)allocator->alloc(allocator, sizeof(name##Entry *) * 8), \
            .bucket_count = 8, \
            .size = 0, \
            .hash = hash, \
            .eq = eq, \
            .allocator = allocator \
        }; \
        return table; \
    } \
    \
    void prefix##_set(name *table, key_type key, value_type value) { \
        u64 hash = table->hash(key); \
        usize bucket_index = hash % table->bucket_count; \
        \
        name##Entry *entry = table->buckets[bucket_index]; \
        while (entry != NULL) { \
            if (table->eq(entry->key, key)) { \
                entry->value = value; \
                return; \
            } \
            entry = entry->next; \
        } \
        \
        name##Entry *new_entry = (name##Entry *)table->allocator->alloc( \
            table->allocator, sizeof(name##Entry)); \
        new_entry->key = key; \
        new_entry->value = value; \
        new_entry->next = table->buckets[bucket_index]; \
        table->buckets[bucket_index] = new_entry; \
        table->size++; \
        \
        if (table->size > table->bucket_count * 3 / 4) { \
            prefix##_grow(table); \
        } \
    } \
    \
    bool prefix##_contains(name *table, key_type key) { \
        u64 hash = table->hash(key); \
        usize bucket_index = hash % table->bucket_count; \
        \
        name##Entry *entry = table->buckets[bucket_index]; \
        while (entry != NULL) { \
            if (table->eq(entry->key, key)) { \
                return true; \
            } \
            entry = entry->next; \
        } \
        return false; \
    } \
    \
    value_type prefix##_get(name *table, key_type key) { \
        u64 hash = table->hash(key); \
        usize bucket_index = hash % table->bucket_count; \
        \
        name##Entry *entry = table->buckets[bucket_index]; \
        while (entry != NULL) { \
            if (table->eq(entry->key, key)) { \
                return entry->value; \
            } \
            entry = entry->next; \
        } \
        ASSERT(false && "Key not found in hash table"); \
        return (value_type){0}; \
    } \
    \
    bool prefix##_remove(name *table, key_type key) { \
        u64 hash = table->hash(key); \
        usize bucket_index = hash % table->bucket_count; \
        \
        name##Entry *entry = table->buckets[bucket_index]; \
        name##Entry *prev = NULL; \
        \
        while (entry != NULL) { \
            if (table->eq(entry->key, key)) { \
                if (prev == NULL) { \
                    table->buckets[bucket_index] = entry->next; \
                } else { \
                    prev->next = entry->next; \
                } \
                table->allocator->free(table->allocator, entry); \
                table->size--; \
                return true; \
            } \
            prev = entry; \
            entry = entry->next; \
        } \
        return false; \
    } \
    \
    void prefix##_reset(name *table) { \
        for (usize i = 0; i < table->bucket_count; i++) { \
            name##Entry *entry = table->buckets[i]; \
            while (entry != NULL) { \
                name##Entry *next = entry->next; \
                table->allocator->free(table->allocator, entry); \
                entry = next; \
            } \
            table->buckets[i] = NULL; \
        } \
        table->size = 0; \
    } \
    \
    void prefix##_free(name *table) { \
        prefix##_reset(table); \
        table->allocator->free(table->allocator, table->buckets); \
        table->buckets = NULL; \
        table->bucket_count = 0; \
    } \

#endif // BASE_DECLARATIONS

// --------------------------------------------------------------------------------------

#ifdef BASE_IMPLEMENTATION
#undef BASE_IMPLEMENTATION

// ---------------
// --- Testing ---
// ---------------

#ifdef TESTS_ENABLED

int TEST_RESULTS(void) {
    printf("\n=== TEST SUMMARY ===\n");
    printf("Tests ran: %d, Failed: %d\n", _tests_ran, _tests_failed);
    printf("Overall: %s\n", (_tests_failed == 0) ? "PASS" : "FAIL");
    return !(_tests_failed == 0);
}

#else // TESTS_ENABLED

int TEST_RESULTS(void) { return 0; }

#endif // TESTS_ENABLED

// -------------------------
// --- Memory Allocation ---
// -------------------------

void *heap_allocator_alloc(Allocator *allocator, usize size) {
    void *ptr = malloc(size);
    ASSERT(ptr != NULL);
    memset(ptr, 0, size);
    return ptr;
}

void *heap_allocator_realloc(Allocator *allocator, void *ptr, usize old_size, usize new_size) {
    void *new_ptr = realloc(ptr, new_size);
    ASSERT(new_ptr != NULL);
    return new_ptr;
}

void heap_allocator_free(Allocator *allocator, void *ptr) {
    free(ptr);
}

Allocator heap_allocator = {
    heap_allocator_alloc,
    heap_allocator_realloc,
    heap_allocator_free
};

void *arena_allocator_alloc(Allocator *allocator, usize size) {
    Arena *arena = (Arena *)allocator;
    return arena_alloc(arena, size);
}

void *arena_allocator_realloc(Allocator *allocator, void *ptr, usize old_size, usize new_size) {
    Arena *arena = (Arena *)allocator;
    return arena_realloc(arena, ptr, old_size, new_size);
}

void arena_allocator_free(Allocator *allocator, void *ptr) {}

Arena arena_new(usize capacity, Allocator *backing_allocator) {
    Arena arena = {
        .allocator = {
            .alloc = arena_allocator_alloc,
            .realloc = arena_allocator_realloc,
            .free = arena_allocator_free
        },
        .backing_allocator = backing_allocator,
        .buffer = (u8 *)backing_allocator->alloc(backing_allocator, capacity),
        .capacity = capacity
    };
    return arena;
}

void *arena_alloc(Arena *arena, usize size) {
    ASSERT(arena->buffer != NULL);
    ASSERT(arena->offset + size <= arena->capacity);
    void *ptr = arena->buffer + arena->offset;
    arena->offset += size;
    arena->last_alloc = ptr;
    memset(ptr, 0, size);
    return ptr;
}

void *arena_realloc(Arena *arena, void *ptr, usize old_size, usize new_size) {
    ASSERT(arena->buffer != NULL);
    ASSERT(ptr != NULL);
    i64 difference = new_size - old_size;

    if (ptr == arena->last_alloc) {
        if (difference <= 0) {
            arena->offset -= difference;
            return ptr;
        }
        else {
            ASSERT(arena->offset + difference <= arena->capacity);
            memset(ptr + old_size, 0, difference);
            arena->offset += difference;
            return ptr;
        }
    }
    else {
        if (difference <= 0) {
            return ptr;
        }
        else {
            void *new_ptr = arena_alloc(arena, new_size);
            memcpy(new_ptr, ptr, old_size);
            arena->last_alloc = new_ptr;
            return new_ptr;
        }
    }
}

void arena_reset(Arena *arena) {
    arena->offset = 0;
}

void arena_free(Arena *arena) {
    arena->backing_allocator->free(arena->backing_allocator, arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}

// ---------------
// --- Strings ---
// ---------------

String string(const char *cstr, Allocator *allocator) {
    usize length = strlen(cstr);
    String str = {
        .buffer = (u8 *)allocator->alloc(allocator, length),
        .length = length,
        .allocator = allocator
    };
    memcpy(str.buffer, cstr, length + 1);
    return str;
}

String string_new(usize length, Allocator *allocator) {
    String str = {
        .buffer = (u8 *)allocator->alloc(allocator, length),
        .length = length,
        .allocator = allocator
    };
    return str;
}

String string_concat(String a, String b, Allocator *allocator) {
    String str = {
        .buffer = (u8 *)allocator->alloc(allocator, a.length + b.length),
        .length = a.length + b.length,
        .allocator = allocator
    };
    memcpy(str.buffer, a.buffer, a.length);
    memcpy(str.buffer + a.length, b.buffer, b.length);
    return str;
}

String string_slice(String string, usize start, usize end) {
    String str = {
        .buffer = string.buffer + start,
        .length = end - start,
        .allocator = NULL
    };
    return str;
}

bool string_eq(String a, String b) {
    if (a.length != b.length) return false;
    for (usize i = 0; i < a.length; i++) {
        if (a.buffer[i] != b.buffer[i]) return false;
    }
    return true;
}

bool string_eq_cstr(String a, const char *cstr) {
    usize length = strlen(cstr);
    if (a.length != length) return false;
    for (usize i = 0; i < a.length; i++) {
        if (a.buffer[i] != cstr[i]) return false;
    }
    return true;
}

u64 string_hash(String str) {
    // FNV-1a hash for strings
    u64 hash = 14695981039346656037ULL; // FNV offset basis
    for (usize i = 0; i < str.length; i++) {
        hash ^= str.buffer[i];
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

void string_free(String *string) {
    ASSERT(string != NULL);
    ASSERT(string->buffer != NULL);
    if (string->allocator == NULL) return;
    string->allocator->free(string->allocator, string->buffer);
    string->buffer = NULL;
    string->length = 0;
}

// -------------------
// --- Hash Tables ---
// -------------------

u64 integer_hash(u64 value) {
    // Simple bit mixing for integers
    value ^= value >> 33;
    value *= 0xff51afd7ed558ccdULL;
    value ^= value >> 33;
    value *= 0xc4ceb9fe1a85ec53ULL;
    value ^= value >> 33;
    return value;
}

bool integer_eq(u64 a, u64 b) {
    return a == b;
}

u64 cstr_hash(const char *str) {
    // FNV-1a hash for strings
    u64 hash = 14695981039346656037ULL; // FNV offset basis
    for (usize i = 0; i < strlen(str); i++) {
        hash ^= str[i];
        hash *= 1099511628211ULL; // FNV prime
    }
    return hash;
}

bool cstr_eq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

#endif // BASE_IMPLEMENTATION
