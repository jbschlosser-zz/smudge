TESTS = check_smudge
check_PROGRAMS = check_smudge
check_smudge_SOURCES = check_smudge.c color_string_tests.c utilities_tests.c line_buffer_tests.c key_binding_tests.c telnet_tests.c esc_sequence_tests.c memleak_checks.c
check_smudge_CFLAGS = @CHECK_CFLAGS@
check_smudge_LDADD = $(top_builddir)/src/libsmudge.a -lncurses -lev @CHECK_LIBS@ $(GUILE_LIBS) $(PCRE_LIBS)
