#ifndef MUD_SESSION_H
#define MUD_SESSION_H

#include "history.h"
#include "input_line.h"
#include "line_buffer.h"
#include "mud_connection.h"
#include "mud_string.h"
#include "scrollback.h"
#include "search.h"

typedef struct {
    mud_connection *connection;
    scrollback *output_data;
    history *hist;
    input_line *input_data;
    search_result last_search_result;
} mud_session;

// Construction/destruction.
mud_session *mud_session_create(mud_connection *connection, scrollback *output_data, history *hist, input_line *input_data);
void mud_session_destroy(mud_session *session);

#endif
