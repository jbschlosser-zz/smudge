TESTS = check_mud
check_PROGRAMS = check_mud
check_mud_SOURCES = check_mud.c color_string_tests.c utilities_tests.c line_buffer_tests.c key_binding_tests.c telnet_tests.c esc_sequence_tests.c memleak_checks.c
check_mud_CFLAGS = @CHECK_CFLAGS@
check_mud_LDADD = $(top_builddir)/src/libmud.a -lncurses @CHECK_LIBS@ $(GUILE_LIBS) $(PCRE_LIBS)
