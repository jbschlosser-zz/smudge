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
#include "history_tests.h"
#include "../src/history.h"

START_TEST(test_history_add_retrieve_single_entry)
{
    history *hist = history_create(line_buffer_create(10));

    // Add the entry.
    char entry_str[] = "first_entry";
    color_string *entry = color_string_create_from_c_string(128, entry_str);
    history_add_entry(hist, entry);

    // Now the current entry should be blank...
    ck_assert_int_eq(history_get_pos(hist), 0);
    ck_assert_int_eq(color_string_length(history_get_current_entry(hist)), 0);

    // ...until the history is moved back to the previous entry.
    history_adjust_pos(hist, 1);
    ck_assert_int_eq(history_get_pos(hist), 1);
    color_string *returned_entry = history_get_current_entry(hist);
    char *returned_entry_str = color_string_to_c_str(returned_entry);
    ck_assert_str_eq(returned_entry_str, entry_str);

    // Clean up.
    free(returned_entry_str);
    color_string_destroy(entry);
    history_destroy(hist);
}
END_TEST

// Suite creation.
Suite *make_history_suite(void)
{
    Suite *s = suite_create("Telnet");
    ADD_TEST_TO_SUITE(test_history_add_retrieve_single_entry, s);

    return s;
}
