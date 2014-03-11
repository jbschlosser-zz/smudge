#ifndef SESSION_H
#define SESSION_H

#include "history.h"
#include "input_line.h"
#include "line_buffer.h"
#include "mud_connection.h"
#include "color_string.h"
#include "scrollback.h"
#include "search.h"

// Stores all state corresponding to a single MUD session.
typedef struct {
    mud_connection *connection;
    scrollback *output_data;
    history *hist;
    input_line *input_data;
    search_result last_search_result;
} session;

// Construction/destruction.
session *session_create(mud_connection *connection, scrollback *output_data, history *hist, input_line *input_data);
void session_destroy(session *sess);

#endif
