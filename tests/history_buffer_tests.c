#include "common_test.h"
#include "history_buffer_tests.h"
#include "../src/history_buffer.h"

// create() tests.
START_TEST(test_history_buffer_create)
{
    history_buffer *buf;
    buf = history_buffer_create(10);
    ck_assert_int_eq(history_buffer_size(buf), 10);
    history_buffer_destroy(buf);
}
END_TEST

// add_entry()/get_entry() tests.
START_TEST(test_history_single_entry)
{
    // Arrange.
    history_buffer *buf;
    buf = history_buffer_create(10);

    // Act.
    const char test_data[] = "hello world";
    history_buffer_add_entry(buf, test_data, strlen(test_data));

    // Assert.
    ck_assert(memcmp(history_buffer_get_entry(buf, 1), test_data, sizeof(char) * strlen(test_data)) == 0);

    // Clean up.
    history_buffer_destroy(buf);
}
END_TEST
START_TEST(test_history_multiple_entries_no_wrap)
{
    // Arrange.
    history_buffer *buf;
    buf = history_buffer_create(5);

    // Act.
    const char test_data1[] = "hello world";
    const char test_data2[] = "this";
    const char test_data3[] = "is a";
    const char test_data4[] = "test";
    history_buffer_add_entry(buf, test_data1, strlen(test_data1));
    history_buffer_add_entry(buf, test_data2, strlen(test_data2));
    history_buffer_add_entry(buf, test_data3, strlen(test_data3));
    history_buffer_add_entry(buf, test_data4, strlen(test_data4));

    // Assert.
    ck_assert(strcmp(history_buffer_get_entry(buf, 1), test_data4) == 0);
    ck_assert(strcmp(history_buffer_get_entry(buf, 2), test_data3) == 0);
    ck_assert(strcmp(history_buffer_get_entry(buf, 3), test_data2) == 0);
    ck_assert(strcmp(history_buffer_get_entry(buf, 4), test_data1) == 0);

    // Clean up.
    history_buffer_destroy(buf);
}
END_TEST
START_TEST(test_history_multiple_entries_with_wrap)
{
    // Arrange.
    history_buffer *buf;
    buf = history_buffer_create(3);

    // Act.
    const char test_data1[] = "hello world";
    const char test_data2[] = "this";
    const char test_data3[] = "is a";
    const char test_data4[] = "test";
    history_buffer_add_entry(buf, test_data1, strlen(test_data1));
    history_buffer_add_entry(buf, test_data2, strlen(test_data2));
    history_buffer_add_entry(buf, test_data3, strlen(test_data3));
    history_buffer_add_entry(buf, test_data4, strlen(test_data4));

    // Assert.
    ck_assert(strcmp(history_buffer_get_entry(buf, 1), test_data4) == 0);
    ck_assert(strcmp(history_buffer_get_entry(buf, 2), test_data3) == 0);
    ck_assert(strcmp(history_buffer_get_entry(buf, 3), test_data2) == 0);
    ck_assert(history_buffer_get_entry(buf, 4) == NULL);

    // Clean up.
    history_buffer_destroy(buf);
}
END_TEST

// Suite creation.
Suite *make_history_buffer_suite(void)
{
    Suite *s = suite_create("History buffer");
    ADD_TEST_TO_SUITE(test_history_buffer_create, s);
    ADD_TEST_TO_SUITE(test_history_single_entry, s);
    ADD_TEST_TO_SUITE(test_history_multiple_entries_no_wrap, s);
    ADD_TEST_TO_SUITE(test_history_multiple_entries_with_wrap, s);

    return s;
}
