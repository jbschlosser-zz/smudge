/*
 * Copyright (C) 2014 Joel Schlosser All Rights Reserved.
 *
 * This file is part of smudge.
 *
 * smudge is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * smudge is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with smudge.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

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
