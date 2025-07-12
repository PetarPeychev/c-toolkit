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

// -------------------------
// --- Memory Allocation ---
// -------------------------

typedef struct Allocator Allocator;

struct Allocator {
    void *(*alloc)(Allocator *allocator, usize size);
    void (*free)(Allocator *allocator, void *ptr);
};

extern Allocator heap_allocator;

typedef struct {
    Allocator allocator;
    Allocator *backing_allocator;
    u8 *buffer;
    usize capacity;
    usize offset;
} Arena;

Arena arena_new(usize capacity, Allocator *backing_allocator);
void *arena_alloc(Arena *arena, usize size);
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
void string_free(String *string);

#endif // BASE_DECLARATIONS

// --------------------------------------------------------------------------------------

#ifdef BASE_IMPLEMENTATION
#undef BASE_IMPLEMENTATION

// -------------------------
// --- Memory Allocation ---
// -------------------------

void *heap_allocator_alloc(Allocator *allocator, usize size) {
    void *ptr = malloc(size);
    ASSERT(ptr != NULL);
    memset(ptr, 0, size);
    return ptr;
}

void heap_allocator_free(Allocator *allocator, void *ptr) {
    free(ptr);
}

Allocator heap_allocator = {heap_allocator_alloc, heap_allocator_free};

void *arena_allocator_alloc(Allocator *allocator, usize size) {
    Arena *arena = (Arena *)allocator;
    return arena_alloc(arena, size);
}

void arena_allocator_free(Allocator *allocator, void *ptr) {}

Arena arena_new(usize capacity, Allocator *backing_allocator) {
    Arena arena = {
        .allocator = {
            .alloc = arena_allocator_alloc,
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
    memset(ptr, 0, size);
    return ptr;
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

void string_free(String *string) {
    if (string->allocator == NULL) return;
    string->allocator->free(string->allocator, string->buffer);
    string->buffer = NULL;
    string->length = 0;
}

#endif // BASE_IMPLEMENTATION
