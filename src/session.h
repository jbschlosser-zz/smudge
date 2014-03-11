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
