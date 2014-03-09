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
START_TEST(test_mud_string_insert_string)
{
    // Arrange.
    mud_string *str = mud_string_create_empty(3);

    // Act.
    const char test_str[] = "this is a test";
    mud_string_append_c_str(str, test_str, strlen(test_str));
    const char test_str2[] = "n insertion";
    mud_string *insert_str = mud_string_create_from_c_string(128, test_str2);
    mud_string_insert(str, 9, mud_string_get_data(insert_str), mud_string_length(insert_str));

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "this is an insertion test";
    ck_assert_str_eq(result_str, expected_result_str);

    // Clean up.
    free(result_str);
    mud_string_destroy(str);
    mud_string_destroy(insert_str);
}
END_TEST
START_TEST(test_mud_string_insert_lots)
{
    // Arrange.
    const char initial_str[] = "abcdef";
    mud_string *str = mud_string_create_from_c_string(3, initial_str);

    // Act.
    mud_char_t char_to_insert = 'x';
    mud_string_insert(str, 0, &char_to_insert, 1);
    mud_string_insert(str, 2, &char_to_insert, 1);
    mud_string_insert(str, 4, &char_to_insert, 1);
    mud_string_insert(str, 6, &char_to_insert, 1);
    mud_string_insert(str, 8, &char_to_insert, 1);
    mud_string_insert(str, 10, &char_to_insert, 1);

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "xaxbxcxdxexf";
    ck_assert_str_eq(result_str, expected_result_str);

    // Clean up.
    free(result_str);
    mud_string_destroy(str);
}
END_TEST
START_TEST(test_mud_string_delete_char_first_index)
{
    // Arrange.
    const char initial_str[] = "abcdef";
    mud_string *str = mud_string_create_from_c_string(6, initial_str);

    // Act.
    mud_string_delete_char(str, 0);

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "bcdef";
    ck_assert_str_eq(result_str, expected_result_str);
    ck_assert_int_eq(mud_string_length(str), 5);

    // Clean up.
    free(result_str);
    mud_string_destroy(str);
}
END_TEST
START_TEST(test_mud_string_delete_char_middle_index)
{
    // Arrange.
    const char initial_str[] = "abcdef";
    mud_string *str = mud_string_create_from_c_string(6, initial_str);

    // Act.
    mud_string_delete_char(str, 2);

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "abdef";
    ck_assert_str_eq(result_str, expected_result_str);
    ck_assert_int_eq(mud_string_length(str), 5);

    // Clean up.
    free(result_str);
    mud_string_destroy(str);
}
END_TEST
START_TEST(test_mud_string_delete_char_last_index)
{
    // Arrange.
    const char initial_str[] = "abcdef";
    mud_string *str = mud_string_create_from_c_string(6, initial_str);

    // Act.
    mud_string_delete_char(str, strlen(initial_str) - 1);

    // Assert.
    char *result_str = mud_string_to_c_str(str);
    const char expected_result_str[] = "abcde";
    ck_assert_str_eq(result_str, expected_result_str);
    ck_assert_int_eq(mud_string_length(str), 5);

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
    ADD_TEST_TO_SUITE(test_mud_string_insert_string, s);
    ADD_TEST_TO_SUITE(test_mud_string_insert_lots, s);
    ADD_TEST_TO_SUITE(test_mud_string_delete_char_first_index, s);
    ADD_TEST_TO_SUITE(test_mud_string_delete_char_middle_index, s);
    ADD_TEST_TO_SUITE(test_mud_string_delete_char_last_index, s);

    return s;
}
