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
#include "utilities_tests.h"
#include "../src/line_buffer.c"

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
