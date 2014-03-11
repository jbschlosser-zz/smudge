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

#ifndef COMMON_TEST_H
#define COMMON_TEST_H

// Convenience macro for adding a test case.
#define ADD_TEST_TO_SUITE(test, suite) \
    {\
        TCase *tc = tcase_create(#test);\
        tcase_add_test(tc, test);\
        suite_add_tcase(s, tc);\
    }\

#endif
