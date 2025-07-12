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

Arena arena_new(Allocator *backing_allocator, usize capacity);
void *arena_alloc(Arena *arena, usize size);
void arena_reset(Arena *arena);
void arena_free(Arena *arena);

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

Arena arena_new(Allocator *backing_allocator, usize capacity) {
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

#endif // BASE_IMPLEMENTATION
