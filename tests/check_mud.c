#include <check.h>
#include <stdlib.h>
#include "line_buffer_tests.h"
#include "mud_string_tests.h"
#include "utilities_tests.h"

int main(void)
{
    int number_failed;
    SRunner *sr = srunner_create(make_utilities_suite());
    srunner_add_suite(sr, make_line_buffer_suite());
    srunner_add_suite(sr, make_mud_string_suite());
    srunner_set_fork_status(sr, CK_NOFORK);
    srunner_set_log(sr, "tests.log");
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
