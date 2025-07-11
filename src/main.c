#include "base.c"

int main(void) {
    Arena arena = arena_new(8);
    int *ptr = arena_alloc(&arena, sizeof(int));
    *ptr = 42;
    float *ptr2 = arena_alloc(&arena, sizeof(float));
    *ptr2 = 3.14;

    printf("%d\n", *ptr);
    printf("%f\n", *ptr2);

    arena_reset(&arena);

    ptr = arena_alloc(&arena, sizeof(int));
    *ptr = 69;
    ptr2 = arena_alloc(&arena, sizeof(float));
    *ptr2 = 4.20;

    printf("%d\n", *ptr);
    printf("%f\n", *ptr2);

    arena_free(&arena);

    printf("%d\n", *ptr);
    printf("%f\n", *ptr2);

    return 0;
}
