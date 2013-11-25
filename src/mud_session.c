#include <stdlib.h>
#include "mud_session.h"

mud_session *mud_session_create(mud_connection *connection, line_buffer *scrollback_data, line_buffer *history_data)
{
    if(!connection) return NULL;
    if(!scrollback_data) return NULL;
    if(!history_data) return NULL;

    mud_session *session = malloc(sizeof(mud_session));
    session->connection = connection;
    session->scrollback_data = scrollback_data;
    session->_scrollback_index = 0;
    session->history_data = history_data;
    session->_history_index = 0;
    session->input_data = mud_string_create_empty(128);

    return session;
}

void mud_session_destroy(mud_session *session)
{
    mud_connection_destroy(session->connection);
    line_buffer_destroy(session->scrollback_data);
    line_buffer_destroy(session->history_data);
    mud_string_destroy(session->input_data);
    free(session);
}

void mud_session_set_scrollback_index(mud_session *session, int index, int window_lines)
{
    // TODO: The behavior at the back of scrollback needs to be thought out a little better.
    // It might be cool to have a dynamic stopping point where there are just enough lines to
    // fill the window. Currently, the max is dependent on the window size, and that doesn't
    // seem to make a lot of sense.
    int scroll_lines_avail = (line_buffer_num_lines(session->scrollback_data) - window_lines);
    if(scroll_lines_avail < 0)
        scroll_lines_avail = 0;
    if(index > scroll_lines_avail)
        index = scroll_lines_avail;
    if(index < 0)
        index = 0;
    session->_scrollback_index = index;
}

int mud_session_get_scrollback_index(mud_session *session)
{
    return session->_scrollback_index;
}

void mud_session_adjust_scrollback_index(mud_session *session, int adjustment, int window_lines)
{
    mud_session_set_scrollback_index(session, session->_scrollback_index + adjustment, window_lines);
}

void mud_session_set_history_index(mud_session *session, int index)
{
    if(index >= line_buffer_num_lines(session->history_data))
        index = line_buffer_num_lines(session->history_data) - 1;
    if(index < 0)
        index = 0;
    session->_history_index = index;
}

int mud_session_get_history_index(mud_session *session)
{
    return session->_history_index;
}

void mud_session_adjust_history_index(mud_session *session, int adjustment)
{
    mud_session_set_history_index(session, session->_history_index + adjustment);
}
