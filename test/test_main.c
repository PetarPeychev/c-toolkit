#define BASE_IMPLEMENTATION
#include "../lib/base.h"

#define TEST_IMPLEMENTATION
#include "../lib/test.h"

TEST(equal_pass) {
    TEST_ASSERT(1 == 1);
}

TEST(equal_fail) {
    TEST_ASSERT(1 == 2);
}

int main(void) {
    TEST_RUN(equal_pass);
    TEST_RUN(equal_fail);

    return TEST_RESULTS();
}
