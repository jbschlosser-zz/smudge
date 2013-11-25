#ifndef MUD_SESSION_H
#define MUD_SESSION_H

#include "line_buffer.h"
#include "mud_connection.h"
#include "mud_string.h"

typedef struct {
    mud_connection *connection;
    line_buffer *scrollback_data;
    int _scrollback_index;
    line_buffer *history_data;
    int _history_index;
    mud_string *input_data;
} mud_session;

// Construction/destruction.
mud_session *mud_session_create(mud_connection *connection, line_buffer *scrollback_data, line_buffer *history_data);
void mud_session_destroy(mud_session *session);

// Member functions.
void mud_session_set_scrollback_index(mud_session *session, int index, int window_lines);
int mud_session_get_scrollback_index(mud_session *session);
void mud_session_adjust_scrollback_index(mud_session *session, int adjustment, int window_lines);

void mud_session_set_history_index(mud_session *session, int index);
int mud_session_get_history_index(mud_session *session);
void mud_session_adjust_history_index(mud_session *session, int adjustment);

#endif
