#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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

#ifdef TEST_ENABLED

static bool _test_failed = false;
static i32 _tests_ran = 0;
static i32 _tests_failed = 0;

#define TEST(name) static void test_##name(void)

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

int TEST_RESULTS(void) {
    printf("\n=== TEST SUMMARY ===\n");
    printf("Tests ran: %d, Failed: %d\n", _tests_ran, _tests_failed);
    printf("Overall: %s\n", (_tests_failed == 0) ? "PASS" : "FAIL");
    return !(_tests_failed == 0);
}

#else // TEST_ENABLED

#define TEST(name) static bool test_##name(void)
#define TEST_ASSERT(condition)
#define TEST_RUN(name)
#define TEST_RUN_ALL()
#define TEST_PASSED true

#endif // TEST_ENABLED

// -------------------------
// --- Memory Allocation ---
// -------------------------

typedef struct {
    u8 *buffer;
    usize capacity;
    usize offset;
} Arena;

Arena arena_new(usize capacity) {
    Arena arena = {
        .buffer = (u8 *)malloc(capacity),
        .capacity = capacity
    };
    ASSERT(arena.buffer != NULL);
    return arena;
}

void *arena_alloc(Arena *arena, usize size) {
    ASSERT(arena->offset + size <= arena->capacity);
    void *ptr = arena->buffer + arena->offset;
    arena->offset += size;
    return ptr;
}

void arena_reset(Arena *arena) {
    arena->offset = 0;
}

void arena_free(Arena *arena) {
    free(arena->buffer);
    arena->buffer = NULL;
    arena->capacity = 0;
    arena->offset = 0;
}


// ---------------
// --- Strings ---
// ---------------

typedef struct {
    u8 *buffer;
    usize length;
} String;

String string_new(Arena *arena, usize length) {
    String string = {
        .buffer = arena_alloc(arena, length),
        .length = length
    };
    return string;
}

String string_from_cstr(Arena *arena, char *cstr) {
    String string = {
        .buffer = arena_alloc(arena, strlen(cstr)),
        .length = strlen(cstr)
    };
    memcpy(string.buffer, cstr, string.length);
    return string;
}

void string_append(String *string, u8 *data, usize length) {
    memcpy(string->buffer + string->length, data, length);
    string->length += length;
}
