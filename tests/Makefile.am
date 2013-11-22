TESTS = check_mud
check_PROGRAMS = check_mud
check_mud_SOURCES = check_mud.c history_buffer_tests.c mud_string_tests.c utilities_tests.c scrollback_buffer_tests.c
check_mud_CFLAGS = @CHECK_CFLAGS@
check_mud_LDADD = $(top_builddir)/src/libmud.a -lncurses @CHECK_LIBS@
