#ifndef COMMON_TEST_H
#define COMMON_TEST_H

// Convenience macro for adding a test case.
#define ADD_TEST_TO_SUITE(test, suite) \
    {\
        TCase *tc = tcase_create(#test);\
        tcase_add_test(tc, test);\
        suite_add_tcase(s, tc);\
    }\

#endif
