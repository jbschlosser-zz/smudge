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

#include <check.h>
#include <stdlib.h>
#include "esc_sequence_tests.h"
#include "line_buffer_tests.h"
#include "color_string_tests.h"
#include "key_binding_tests.h"
#include "memleak_checks.h"
#include "telnet_tests.h"
#include "utilities_tests.h"

int main(void)
{
    int number_failed;
    SRunner *sr = srunner_create(make_utilities_suite());
    srunner_add_suite(sr, make_line_buffer_suite());
    srunner_add_suite(sr, make_color_string_suite());
    srunner_add_suite(sr, make_key_binding_suite());
    srunner_add_suite(sr, make_memleak_checks_suite());
    srunner_add_suite(sr, make_telnet_suite());
    srunner_add_suite(sr, make_esc_sequence_suite());
    //srunner_set_fork_status(sr, CK_NOFORK); // Use this for debugging.
    srunner_set_log(sr, "tests.log");
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
