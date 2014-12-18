lib_LIBRARIES = libsmudge.a
libsmudge_a_CFLAGS = $(GUILE_CFLAGS)
libsmudge_a_SOURCES = mud_connection.c user_interface.c socket_ops.c color_string.c session.c line_buffer.c search.c history.c scrollback.c input_line.c keys.c telnet.c esc_sequence.c guile.c action.c
bin_PROGRAMS = smudge
smudge_CFLAGS = $(GUILE_CFLAGS) $(PCRE_CFLAGS)
smudge_SOURCES = main.c
smudge_LDADD = libsmudge.a -lncurses -luv $(GUILE_LIBS) $(PCRE_LIBS)
