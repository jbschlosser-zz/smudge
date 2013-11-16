#include "common_test.h"
#include "utilities_tests.h"
#include "../src/utilities.h"

// modulo() tests.
START_TEST(test_modulo_positive_numbers)
{
    ck_assert_int_eq(modulo(3, 5), 3);
    ck_assert_int_eq(modulo(5, 3), 2);
}
END_TEST
START_TEST(test_modulo_negative_dividend)
{
    ck_assert_int_eq(modulo(-3, 5), 2);
    ck_assert_int_eq(modulo(-5, 3), 1);
}
END_TEST
START_TEST(test_modulo_negative_divisor)
{
    ck_assert_int_eq(modulo(3, -5), 3);
    ck_assert_int_eq(modulo(5, -3), 2);
}
END_TEST

// Suite creation.
Suite *make_utilities_suite(void)
{
    Suite *s = suite_create("Utilities");
    ADD_TEST_TO_SUITE(test_modulo_positive_numbers, s);
    ADD_TEST_TO_SUITE(test_modulo_negative_dividend, s);
    ADD_TEST_TO_SUITE(test_modulo_negative_divisor, s);

    return s;
}
