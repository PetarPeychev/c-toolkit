#ifndef TEST_DECLARATIONS
#define TEST_DECLARATIONS

#include "base.h"

#define TEST(name) static void test_##name(void)
int TEST_RESULTS(void);

#ifdef TEST_ENABLED

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

#else // TEST_ENABLED

#define TEST_ASSERT(condition)
#define TEST_RUN(name)

#endif // TEST_ENABLED

#endif // BASE_DECLARATIONS

// --------------------------------------------------------------------------------------

#ifdef TEST_IMPLEMENTATION
#undef TEST_IMPLEMENTATION

// ---------------
// --- Testing ---
// ---------------

#ifdef TEST_ENABLED

bool _test_failed = false;
i32 _tests_ran = 0;
i32 _tests_failed = 0;

int TEST_RESULTS(void) {
    printf("\n=== TEST SUMMARY ===\n");
    printf("Tests ran: %d, Failed: %d\n", _tests_ran, _tests_failed);
    printf("Overall: %s\n", (_tests_failed == 0) ? "PASS" : "FAIL");
    return !(_tests_failed == 0);
}

#else // TEST_ENABLED

int TEST_RESULTS(void) { return 0; }

#endif // TEST_ENABLED

#endif // TEST_IMPLEMENTATION
