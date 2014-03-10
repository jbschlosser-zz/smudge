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
