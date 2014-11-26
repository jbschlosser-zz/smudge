/*
 * Copyright (C) 2014 Joel Schlosser All Rights Reserved.
 *
 * This file is part of smudge.
 *
 * smudge is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * smudge is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with smudge.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include "keys.h"
#include "session.h"

session *session_create(
    mud_connection *connection,
    scrollback *output_data,
    history *hist,
    input_line *input_data,
    key_binding_table *bindings)
{
    if(!connection) return NULL;
    if(!output_data) return NULL;
    if(!hist) return NULL;
    if(!input_data) return NULL;
    if(!bindings) return NULL;

    session *sess = malloc(sizeof(session));
    sess->connection = connection;
    sess->output_data = output_data;
    sess->hist = hist;
    sess->input_data = input_data;
    sess->bindings = bindings;

    sess->last_search_result.line_number = -1;
    sess->last_search_result.begin_index = -1;
    sess->last_search_result.end_index = -1;

    sess->prev = NULL;
    sess->next = NULL;

    return sess;
}

void session_destroy(session *sess)
{
    if(!sess) return;

    mud_connection_destroy(sess->connection);
    scrollback_destroy(sess->output_data);
    history_destroy(sess->hist);
    input_line_destroy(sess->input_data);
    key_binding_table_destroy(sess->bindings);
    free(sess);
}
