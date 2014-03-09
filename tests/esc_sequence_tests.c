#include <stdlib.h>
#include "common_test.h"
#include "esc_sequence_tests.h"
#include "../src/esc_sequence.h"

START_TEST(test_esc_sequence_clear)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);

    // Act.
    esc_sequence_clear(seq);

    // Assert.
    ck_assert_int_eq(seq->len, 0);
    ck_assert_int_eq(seq->ready, 0);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST
START_TEST(test_esc_sequence_non_esc_begin)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();

    // Act.
    int i;
    for(i = 0; i < 255; ++i) {
        if(i == ESCAPE_SEQUENCE_BEGIN) continue;
        esc_sequence_update(seq, i); // Not ESCAPE_SEQUENCE_BEGIN.
    }
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);
    esc_sequence_update(seq, 0x1);

    // Assert.
    ck_assert_int_eq(seq->len, 2);
    ck_assert_int_eq(seq->ready, 0);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST
START_TEST(test_esc_sequence_normal_length)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();

    // Act.
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);
    esc_sequence_update(seq, 0x1);
    esc_sequence_update(seq, 0x2);
    esc_sequence_update(seq, 0x3);
    esc_sequence_update(seq, ESCAPE_SEQUENCE_END);

    // Assert.
    ck_assert_int_eq(seq->len, 5);
    ck_assert_int_eq(seq->ready, 1);
    char expected_data[] = {ESCAPE_SEQUENCE_BEGIN, 0x1, 0x2, 0x3,
        ESCAPE_SEQUENCE_END, 0x00};
    ck_assert_str_eq(seq->data, expected_data);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST
START_TEST(test_esc_sequence_unterminated)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();

    // Act.
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);
    esc_sequence_update(seq, 0x1);
    esc_sequence_update(seq, 0x2);

    // Assert.
    ck_assert_int_eq(seq->len, 3);
    ck_assert_int_eq(seq->ready, 0);
    char expected_data[] = {ESCAPE_SEQUENCE_BEGIN, 0x1, 0x2};
    ck_assert(memcmp(seq->data, expected_data, sizeof(expected_data)) == 0);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST
START_TEST(test_esc_sequence_at_max_length)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();

    // Act.
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);
    char i;
    for(i = 1; i <= ESCAPE_SEQUENCE_MAX_SIZE - 2; ++i) {
        esc_sequence_update(seq, i);
    }
    esc_sequence_update(seq, ESCAPE_SEQUENCE_END);

    // Assert.
    ck_assert_int_eq(seq->len, 15);
    ck_assert_int_eq(seq->ready, 1);
    char expected_data[] = {ESCAPE_SEQUENCE_BEGIN, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6,
        0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, ESCAPE_SEQUENCE_END, 0x00};
    ck_assert_str_eq(seq->data, expected_data);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST
START_TEST(test_esc_sequence_past_max_length)
{
    // Arrange.
    esc_sequence *seq = esc_sequence_create();

    // Act.
    esc_sequence_update(seq, ESCAPE_SEQUENCE_BEGIN);
    char i;
    for(i = 1; i <= ESCAPE_SEQUENCE_MAX_SIZE - 1; ++i) {
        esc_sequence_update(seq, i);
    }
    esc_sequence_update(seq, ESCAPE_SEQUENCE_END);

    // Assert.
    ck_assert_int_eq(seq->len, 0);
    ck_assert_int_eq(seq->ready, 0);

    // Clean up.
    esc_sequence_destroy(seq);
}
END_TEST

// Suite creation.
Suite *make_esc_sequence_suite(void)
{
    Suite *s = suite_create("ESC Sequence");
    ADD_TEST_TO_SUITE(test_esc_sequence_clear, s);
    ADD_TEST_TO_SUITE(test_esc_sequence_non_esc_begin, s);
    ADD_TEST_TO_SUITE(test_esc_sequence_normal_length, s);
    ADD_TEST_TO_SUITE(test_esc_sequence_unterminated, s);
    ADD_TEST_TO_SUITE(test_esc_sequence_at_max_length, s);
    ADD_TEST_TO_SUITE(test_esc_sequence_past_max_length, s);

    return s;
}
