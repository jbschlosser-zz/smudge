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
#include "telnet_tests.h"
#include "../src/telnet.h"

START_TEST(test_telnet_clear_cmd)
{
    // Arrange.
    telnet *tel = telnet_create();
    telnet_update(tel, TELNET_IAC);

    // Act.
    telnet_clear_cmd(tel);

    // Assert.
    ck_assert_int_eq(tel->cmd_len, 0);
    ck_assert_int_eq(tel->cmd_ready, 0);

    // Clean up.
    telnet_destroy(tel);
}
END_TEST
START_TEST(test_telnet_double_iac)
{
    // Arrange.
    telnet *tel = telnet_create();

    // Act.
    telnet_update(tel, TELNET_IAC);
    telnet_update(tel, TELNET_IAC);

    // Assert.
    ck_assert_int_eq(tel->cmd_len, 2);
    ck_assert_int_eq(tel->cmd_ready, 1);
    unsigned char expected_cmd[] = {TELNET_IAC, TELNET_IAC};
    ck_assert(memcmp(tel->cmd, expected_cmd, sizeof(expected_cmd)) == 0);

    // Clean up.
    telnet_destroy(tel);
}
END_TEST
START_TEST(test_telnet_single_byte_commands)
{
    unsigned char byte;
    for(byte = TELNET_NOP; byte <= TELNET_GA; ++byte) {
        // Arrange.
        telnet *tel = telnet_create();

        // Act.
        telnet_update(tel, TELNET_IAC);
        telnet_update(tel, byte);

        // Assert.
        ck_assert_int_eq(tel->cmd_len, 2);
        ck_assert_int_eq(tel->cmd_ready, 1);
        unsigned char expected_cmd[] = {TELNET_IAC, byte};
        ck_assert(memcmp(tel->cmd, expected_cmd, sizeof(expected_cmd)) == 0);

        // Clean up.
        telnet_destroy(tel);
    }
}
END_TEST
START_TEST(test_telnet_double_byte_commands)
{
    unsigned char byte;
    for(byte = TELNET_WILL; byte <= TELNET_DONT; ++byte) {
        // Arrange.
        telnet *tel = telnet_create();

        // Act.
        telnet_update(tel, TELNET_IAC);
        telnet_update(tel, byte);
        telnet_update(tel, 0x50); // arbitrary

        // Assert.
        ck_assert_int_eq(tel->cmd_len, 3);
        ck_assert_int_eq(tel->cmd_ready, 1);
        unsigned char expected_cmd[] = {TELNET_IAC, byte, 0x50};
        ck_assert(memcmp(tel->cmd, expected_cmd, sizeof(expected_cmd)) == 0);

        // Clean up.
        telnet_destroy(tel);
    }
}
END_TEST
START_TEST(test_telnet_sub_negotiation)
{
    // Arrange.
    telnet *tel = telnet_create();

    // Act.
    telnet_update(tel, TELNET_IAC);
    telnet_update(tel, TELNET_SB);
    telnet_update(tel, 0x01);
    telnet_update(tel, 0x02);
    telnet_update(tel, 0x03);
    telnet_update(tel, 0x04);
    telnet_update(tel, 0x05);
    telnet_update(tel, 0x06);
    telnet_update(tel, TELNET_IAC);
    telnet_update(tel, TELNET_SE);

    // Assert.
    ck_assert_int_eq(tel->cmd_len, 10);
    ck_assert_int_eq(tel->cmd_ready, 1);
    unsigned char expected_cmd[] =
        {TELNET_IAC, TELNET_SB, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, TELNET_IAC, TELNET_SE};
    ck_assert(memcmp(tel->cmd, expected_cmd, sizeof(expected_cmd)) == 0);

    // Clean up.
    telnet_destroy(tel);
}
END_TEST
START_TEST(test_telnet_past_max_command_size)
{
    // Arrange.
    telnet *tel = telnet_create();

    // Act.
    telnet_update(tel, TELNET_IAC);
    telnet_update(tel, TELNET_SB);

    // Write the max number of bytes supported...
    int i;
    for(i = 0; i < TELNET_MAX_COMMAND_SIZE - 2; ++i) {
        telnet_update(tel, 0x50);
    }

    // ... then write one more.
    telnet_update(tel, 0x50);

    // Assert.
    ck_assert_int_eq(tel->cmd_len, 0);
    ck_assert_int_eq(tel->cmd_ready, 0);

    // Clean up.
    telnet_destroy(tel);
}
END_TEST

// Suite creation.
Suite *make_telnet_suite(void)
{
    Suite *s = suite_create("Telnet");
    ADD_TEST_TO_SUITE(test_telnet_clear_cmd, s);
    ADD_TEST_TO_SUITE(test_telnet_double_iac, s);
    ADD_TEST_TO_SUITE(test_telnet_single_byte_commands, s);
    ADD_TEST_TO_SUITE(test_telnet_double_byte_commands, s);
    ADD_TEST_TO_SUITE(test_telnet_sub_negotiation, s);
    ADD_TEST_TO_SUITE(test_telnet_past_max_command_size, s);

    return s;
}
