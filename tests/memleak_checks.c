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

#include "common_test.h"
#include "memleak_checks.h"
#include "../src/session.h"

START_TEST(create_and_destroy_everything)
{
    mud_connection *conn = mud_connection_create(REAL_SOCKET_OPS);
    mud_connection_destroy(conn);

    scrollback *scroll = scrollback_create(line_buffer_create(20));
    scrollback_destroy(scroll);

    history *hist = history_create(line_buffer_create(10));
    history_destroy(hist);

    input_line *input = input_line_create();
    input_line_destroy(input);
}
END_TEST

Suite *make_memleak_checks_suite(void)
{
    Suite *s = suite_create("Memory leak checks");
    ADD_TEST_TO_SUITE(create_and_destroy_everything, s);

    return s;
}
