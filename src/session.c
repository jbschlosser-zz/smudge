#include <stdlib.h>
#include "session.h"

session *session_create(mud_connection *connection, scrollback *output_data, history *hist, input_line *input_data)
{
    if(!connection) return NULL;
    if(!output_data) return NULL;
    if(!hist) return NULL;
    if(!input_data) return NULL;

    session *sess = malloc(sizeof(session));
    sess->connection = connection;
    sess->output_data = output_data;
    sess->hist = hist;
    sess->input_data = input_data;

    sess->last_search_result.line_number = -1;
    sess->last_search_result.begin_index = -1;
    sess->last_search_result.end_index = -1;

    return sess;
}

void session_destroy(session *sess)
{
    if(!sess) return;

    mud_connection_destroy(sess->connection);
    scrollback_destroy(sess->output_data);
    history_destroy(sess->hist);
    input_line_destroy(sess->input_data);
    free(sess);
}
