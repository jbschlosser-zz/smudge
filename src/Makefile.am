lib_LIBRARIES = libmud.a
libmud_a_CFLAGS = $(GUILE_CFLAGS)
libmud_a_SOURCES = mud_connection.c mud_window.c user_interface.c socket_ops.c window_ops.c color_string.c session.c line_buffer.c search.c history.c scrollback.c input_line.c keys.c telnet.c esc_sequence.c
bin_PROGRAMS = mud
mud_CFLAGS = $(GUILE_CFLAGS) $(PCRE_CFLAGS)
mud_SOURCES = main.c
mud_LDADD = libmud.a -lncurses $(GUILE_LIBS) $(PCRE_LIBS)
