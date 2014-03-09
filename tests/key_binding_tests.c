#include <stdlib.h>
#include "common_test.h"
#include "key_binding_tests.h"
#include "../src/keys.h"

START_TEST(test_get_binding_when_none_set)
{
    // Arrange.
    key_binding_table *kb_table = key_binding_table_create();

    // Act.
    key_binding binding = key_binding_table_get_binding(kb_table, 1);

    // Assert.
    ck_assert_ptr_eq(binding, NULL);

    // Cleanup.
    key_binding_table_destroy(kb_table);
}
END_TEST
START_TEST(test_get_binding_when_set)
{
    // Arrange.
    key_binding_table *kb_table = key_binding_table_create();

    // Act.
    key_binding_table_set_binding(kb_table, 1, key_binding_do_nothing);
    key_binding binding = key_binding_table_get_binding(kb_table, 1);

    // Assert.
    ck_assert_ptr_eq(binding, key_binding_do_nothing);

    // Cleanup.
    key_binding_table_destroy(kb_table);
}
END_TEST
START_TEST(test_get_binding_when_multiple_entries_with_same_hash)
{
    // Arrange.
    key_binding_table *kb_table = key_binding_table_create();

    // Act.
    key_binding_table_set_binding(kb_table, 1, key_binding_do_nothing);
    key_binding_table_set_binding(kb_table, 1 + kb_table->_buckets, key_binding_page_up);

    // Assert.
    ck_assert_ptr_eq(key_binding_table_get_binding(kb_table, 1), key_binding_do_nothing);
    ck_assert_ptr_eq(key_binding_table_get_binding(kb_table, 1 + kb_table->_buckets), key_binding_page_up);

    // Cleanup.
    key_binding_table_destroy(kb_table);
}
END_TEST
START_TEST(test_get_binding_when_multiple_unrelated_entries)
{
    // Arrange.
    key_binding_table *kb_table = key_binding_table_create();

    // Act.
    key_binding_table_set_binding(kb_table, 1, key_binding_do_nothing);
    key_binding_table_set_binding(kb_table, 2, key_binding_page_up);
    key_binding_table_set_binding(kb_table, 3, key_binding_page_down);

    // Assert.
    ck_assert_ptr_eq(key_binding_table_get_binding(kb_table, 1), key_binding_do_nothing);
    ck_assert_ptr_eq(key_binding_table_get_binding(kb_table, 2), key_binding_page_up);
    ck_assert_ptr_eq(key_binding_table_get_binding(kb_table, 3), key_binding_page_down);

    // Cleanup.
    key_binding_table_destroy(kb_table);
}
END_TEST

// Suite creation.
Suite *make_key_binding_suite(void)
{
    Suite *s = suite_create("Key bindings");
    ADD_TEST_TO_SUITE(test_get_binding_when_none_set, s);
    ADD_TEST_TO_SUITE(test_get_binding_when_set, s);
    ADD_TEST_TO_SUITE(test_get_binding_when_multiple_entries_with_same_hash, s);
    ADD_TEST_TO_SUITE(test_get_binding_when_multiple_unrelated_entries, s);

    return s;
}
