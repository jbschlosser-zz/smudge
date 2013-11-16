#include "common_test.h"
#include "window_buffer_tests.h"
#include "../src/window_buffer.h"

// create() tests.
START_TEST(test_window_buffer_create)
{
    window_buffer *buf;
    buf = window_buffer_create(10, 1, 1);
    ck_assert_int_eq(window_buffer_size(buf), 10);
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_create_zero_size)
{
    window_buffer *buf;
    buf = window_buffer_create(0, 1, 1);
    ck_assert(buf == NULL);
}
END_TEST
START_TEST(test_window_buffer_create_negative_size)
{
    window_buffer *buf;
    buf = window_buffer_create(-5, 1, 1);
    ck_assert(buf == NULL);
}
END_TEST
START_TEST(test_window_buffer_null_checks)
{
    window_buffer_destroy(NULL);

    ck_assert_int_eq(window_buffer_size(NULL), 0);

    ck_assert_int_eq(window_buffer_used(NULL), 0);

    mud_char_t source[2];
    window_buffer_write(NULL, source, 2);

    window_buffer *buf = window_buffer_create(5, 1, 1);
    window_buffer_write(buf, NULL, 5);
    window_buffer_destroy(buf);
}
END_TEST

// write() and read() tests.
START_TEST(test_window_buffer_write_read_no_newline_no_scrollback)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2); // 2x2 window.
    mud_char_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    window_buffer_write(buf, test_data, 9);

    // Act.
    mud_char_t dest[2*2];
    int return_value = window_buffer_read(buf, dest, 2*2, 0);

    // Assert.
    ck_assert_int_eq(return_value, 3);
    mud_char_t expected_dest[] = {7, 8, 9};
    ck_assert(memcmp(dest, expected_dest, return_value * sizeof(mud_char_t)) == 0);

    // Clean up.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_write_read_no_newline_with_scrollback)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2); // 2x2 window.
    mud_char_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    window_buffer_write(buf, test_data, 9);

    // Act/Assert.
    // 1 line scrollback.
    mud_char_t dest[2*2];
    int return_value = window_buffer_read(buf, dest, 2*2, 1);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest1[] = {5, 6, 7, 8};
    ck_assert(memcmp(dest, expected_dest1, return_value * sizeof(mud_char_t)) == 0);

    // 2 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 2);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest2[] = {3, 4, 5, 6};
    ck_assert(memcmp(dest, expected_dest2, return_value * sizeof(mud_char_t)) == 0);

    // 3 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 3);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest3[] = {1, 2, 3, 4};
    ck_assert(memcmp(dest, expected_dest3, return_value * sizeof(mud_char_t)) == 0);

    // 4 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 4);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest4[] = {1, 2, 3, 4};
    ck_assert(memcmp(dest, expected_dest4, return_value * sizeof(mud_char_t)) == 0);

    // Clean up.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_write_read_with_newline_no_scrollback)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 3, 3);
    mud_char_t test_data[] = {1, 2, 3, '\n', 4, 5};
    window_buffer_write(buf, test_data, 6);

    // Act.
    mud_char_t dest[3*3];
    int return_value = window_buffer_read(buf, dest, 3*3, 0);

    // Assert.
    ck_assert_int_eq(return_value, 8);
    mud_char_t expected_dest[] = {1, 2, 3, '\n', '\0', '\0', 4, 5};
    ck_assert(memcmp(dest, expected_dest, return_value * sizeof(mud_char_t)) == 0);

    // Clean up.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_write_read_with_newline_with_scrollback)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2); // 2x2 window.
    mud_char_t test_data[] = {1, 2, '\n', 4, '\n', 6, 7, 8, 9};
    window_buffer_write(buf, test_data, 9);

    // Act/Assert.
    // 1 line scrollback.
    mud_char_t dest[2*2];
    int return_value = window_buffer_read(buf, dest, 2*2, 1);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest1[] = {6, 7, 8, 9};
    ck_assert(memcmp(dest, expected_dest1, return_value * sizeof(mud_char_t)) == 0);

    // 2 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 2);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest2[] = {4, '\n', 6, 7};
    ck_assert(memcmp(dest, expected_dest2, return_value * sizeof(mud_char_t)) == 0);

    // 3 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 3);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest3[] = {'\n', '\0', 4, '\n'};
    ck_assert(memcmp(dest, expected_dest3, return_value * sizeof(mud_char_t)) == 0);

    // 4 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 4);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest4[] = {1, 2, '\n', '\0'};
    ck_assert(memcmp(dest, expected_dest4, return_value * sizeof(mud_char_t)) == 0);

    // 5 lines scrollback.
    return_value = window_buffer_read(buf, dest, 2*2, 5);
    ck_assert_int_eq(return_value, 4);
    mud_char_t expected_dest5[] = {1, 2, '\n', '\0'};
    ck_assert(memcmp(dest, expected_dest5, return_value * sizeof(mud_char_t)) == 0);

    // Clean up.
    window_buffer_destroy(buf);
}
END_TEST

// refit() tests.
START_TEST(test_window_buffer_refit_2x2_to_3x3)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2); // 2x2 window.
    mud_char_t test_data[] = {1, 2, 3, '\n', 4, 5};
    window_buffer_write(buf, test_data, 6);

    // Act.
    window_buffer_refit(buf, 3, 3); // 3x3 window.
    mud_char_t dest[10];
    int return_value = window_buffer_read(buf, dest, 3*3, 0);

    // Assert.
    ck_assert_int_eq(return_value, 8);
    mud_char_t expected_dest[] = {1, 2, 3, '\n', '\0', '\0', 4, 5};
    ck_assert(memcmp(dest, expected_dest, return_value * sizeof(mud_char_t)) == 0);

    // Clean up.
    window_buffer_destroy(buf);
}
END_TEST

// used() tests.
START_TEST(test_window_buffer_used_empty_buffer)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(7, 1, 1);

    // Act.
    int return_value = window_buffer_used(buf);

    // Assert.
    ck_assert_int_eq(return_value, 0);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_used_no_wrap)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(7, 1, 1);
    mud_char_t test_data[] = {1, 2, 3, 4, 5};
    window_buffer_write(buf, test_data, 5);

    // Act.
    int return_value = window_buffer_used(buf);

    // Assert.
    ck_assert_int_eq(return_value, 5);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_used_wrap)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(7, 1, 1);
    mud_char_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    window_buffer_write(buf, test_data, 10);

    // Act.
    int return_value = window_buffer_used(buf);

    // Assert.
    ck_assert_int_eq(return_value, 7);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_used_multiple_writes)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 1, 1);
    mud_char_t test_data[] = {1, 2, 3};
    window_buffer_write(buf, test_data, 3);
    mud_char_t test_data2[] = {4, 5, 6, 7};
    window_buffer_write(buf, test_data2, 4);

    // Act.
    int return_value = window_buffer_used(buf);

    // Assert.
    ck_assert_int_eq(return_value, 7);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST

// num_lines() tests.
START_TEST(test_window_buffer_num_lines_full_lines)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2);
    mud_char_t test_data[] = {1, 2, 3, 4, 5, 6};
    window_buffer_write(buf, test_data, 6);

    // Act.
    int return_value = window_buffer_num_lines(buf);

    // Assert.
    ck_assert_int_eq(return_value, 3);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_num_lines_non_full_line)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2);
    mud_char_t test_data[] = {1, 2, 3, 4, 5};
    window_buffer_write(buf, test_data, 5);

    // Act.
    int return_value = window_buffer_num_lines(buf);

    // Assert.
    ck_assert_int_eq(return_value, 3);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST
START_TEST(test_window_buffer_wrapped)
{
    // Arrange.
    window_buffer *buf = window_buffer_create(10, 2, 2);
    mud_char_t test_data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    window_buffer_write(buf, test_data, 13);

    // Act.
    int return_value = window_buffer_num_lines(buf);

    // Assert.
    ck_assert_int_eq(return_value, 5);

    // Cleanup.
    window_buffer_destroy(buf);
}
END_TEST

// Suite creation.
Suite *make_window_buffer_suite(void)
{
    Suite *s = suite_create("Window buffer");
    ADD_TEST_TO_SUITE(test_window_buffer_create, s);
    ADD_TEST_TO_SUITE(test_window_buffer_create_zero_size, s);
    ADD_TEST_TO_SUITE(test_window_buffer_create_negative_size, s);
    ADD_TEST_TO_SUITE(test_window_buffer_null_checks, s);
    ADD_TEST_TO_SUITE(test_window_buffer_write_read_no_newline_no_scrollback, s);
    ADD_TEST_TO_SUITE(test_window_buffer_write_read_no_newline_with_scrollback, s);
    ADD_TEST_TO_SUITE(test_window_buffer_write_read_with_newline_no_scrollback, s);
    ADD_TEST_TO_SUITE(test_window_buffer_write_read_with_newline_with_scrollback, s);
    ADD_TEST_TO_SUITE(test_window_buffer_refit_2x2_to_3x3, s);
    ADD_TEST_TO_SUITE(test_window_buffer_used_empty_buffer, s);
    ADD_TEST_TO_SUITE(test_window_buffer_used_no_wrap, s);
    ADD_TEST_TO_SUITE(test_window_buffer_used_wrap, s);
    ADD_TEST_TO_SUITE(test_window_buffer_used_multiple_writes, s);
    ADD_TEST_TO_SUITE(test_window_buffer_num_lines_full_lines, s);
    ADD_TEST_TO_SUITE(test_window_buffer_num_lines_non_full_line, s);
    ADD_TEST_TO_SUITE(test_window_buffer_wrapped, s);

    return s;
}
