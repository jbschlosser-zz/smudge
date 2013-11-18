#include "common_test.h"
#include "scrollback_buffer_tests.h"
#include "../src/scrollback_buffer.h"

// create() tests.
START_TEST(test_scrollback_buffer_create)
{
    scrollback_buffer *buf;
    buf = scrollback_buffer_create(10);
    ck_assert_int_eq(scrollback_buffer_max_lines(buf), 10);
    scrollback_buffer_destroy(buf);
}
END_TEST

START_TEST(test_scrollback_buffer_add_data)
{
    // Arrange.
    scrollback_buffer *buf;
    buf = scrollback_buffer_create(4);

    // Act.
    const mud_char_t test_str[] = {'h', 'e', 'l', 'l', 'o', '\n', 'w', 'o', 'r', 'l', 'd', '\n', 't', 'h', 'i', 's', '\n', 'i', 's', '\n', 'a', '\n', 't', 'e', 's', 't'};
    scrollback_buffer_write(buf, test_str, 26);

    // Assert.
    mud_string *expected_line0 = mud_string_create_from_c_string(256, "test");
    ck_assert(memcmp(scrollback_buffer_get_line(buf, 0)->_data, expected_line0->_data, sizeof(mud_char_t) * mud_string_length(expected_line0)) == 0);
    mud_string *expected_line1 = mud_string_create_from_c_string(256, "a");
    ck_assert(memcmp(scrollback_buffer_get_line(buf, 1)->_data, expected_line1->_data, sizeof(mud_char_t) * mud_string_length(expected_line1)) == 0);
    mud_string *expected_line2 = mud_string_create_from_c_string(256, "is");
    ck_assert(memcmp(scrollback_buffer_get_line(buf, 2)->_data, expected_line2->_data, sizeof(mud_char_t) * mud_string_length(expected_line2)) == 0);
    mud_string *expected_line3 = mud_string_create_from_c_string(256, "this");
    ck_assert(memcmp(scrollback_buffer_get_line(buf, 3)->_data, expected_line3->_data, sizeof(mud_char_t) * mud_string_length(expected_line3)) == 0);

    // Clean up.
    mud_string_destroy(expected_line0);
    mud_string_destroy(expected_line1);
    mud_string_destroy(expected_line2);
    mud_string_destroy(expected_line3);
    scrollback_buffer_destroy(buf);
}
END_TEST

// Suite creation.
Suite *make_scrollback_buffer_suite(void)
{
    Suite *s = suite_create("Scrollback buffer");
    ADD_TEST_TO_SUITE(test_scrollback_buffer_create, s);
    ADD_TEST_TO_SUITE(test_scrollback_buffer_add_data, s);

    return s;
}
