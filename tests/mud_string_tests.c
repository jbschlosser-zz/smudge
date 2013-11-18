#include <stdlib.h>
#include "common_test.h"
#include "mud_string_tests.h"
#include "../src/mud_string.h"

START_TEST(test_mud_string_create_empty)
{
    mud_string *str = mud_string_create_empty(10);
    ck_assert_int_eq(mud_string_length(str), 0);
    mud_string_destroy(str);
}
END_TEST
START_TEST(test_mud_string_append_multiple_allocations)
{
    // Arrange.
    mud_string *str = mud_string_create_empty(3);

    // Act.
    const char test_str[] = "this is a test";
    mud_string_append_c_str(str, test_str, strlen(test_str));
    const char test_str2[] = " more contents";
    mud_string_append_c_str(str, test_str2, strlen(test_str2));

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "this is a test more contents";
    ck_assert_str_eq(result_str, expected_result_str);

    // Clean up.
    free(result_str);
    mud_string_destroy(str);
}
END_TEST

// Suite creation.
Suite *make_mud_string_suite(void)
{
    Suite *s = suite_create("Mud string");
    ADD_TEST_TO_SUITE(test_mud_string_create_empty, s);
    ADD_TEST_TO_SUITE(test_mud_string_append_multiple_allocations, s);

    return s;
}
