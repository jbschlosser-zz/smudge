lib_LIBRARIES = libmud.a
libmud_a_CFLAGS = $(GUILE_CFLAGS)
libmud_a_SOURCES = mud_connection.c mud_window.c mud_ui.c socket_ops.c window_ops.c mud_char.c utilities.c mud_string.c mud_session.c line_buffer.c
bin_PROGRAMS = mud
mud_CFLAGS = $(GUILE_CFLAGS)
mud_SOURCES = main.c
mud_LDADD = libmud.a -lncurses $(GUILE_LIBS)
