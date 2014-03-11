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
#include "history.h"

history *history_create(line_buffer *data)
{
    if(!data) return NULL;

    history *hist = malloc(sizeof(history));
    hist->_data = data;
    hist->_history_index = 0;

    return hist;
}

void history_destroy(history *hist)
{
    if(!hist) return;

    line_buffer_destroy(hist->_data);
    free(hist);
}

void history_add_entry(history *hist, color_string *entry)
{
    if(!hist) return;
    if(!entry) return;

    line_buffer_write(hist->_data, color_string_get_data(entry), color_string_length(entry));
    color_char newline = '\n';
    line_buffer_write(hist->_data, &newline, 1);

    // Adjust the index so that, if not pointing to the most recent entry, the current stays
    // the same.
    if(hist->_history_index != 0)
        ++hist->_history_index;
}

color_string *history_get_current_entry(history *hist)
{
    if(!hist) return NULL;

    return line_buffer_get_line_relative_to_current(hist->_data, hist->_history_index);
}

int history_get_pos(history *hist)
{
    if(!hist) return -1;

    return hist->_history_index;
}

void history_set_pos(history *hist, int index)
{
    if(!hist) return;
    if(index < 0) return;

    hist->_history_index = index;
    if(hist->_history_index < 0)
        hist->_history_index = 0;
    if(hist->_history_index >= line_buffer_num_lines(hist->_data))
        hist->_history_index = (line_buffer_num_lines(hist->_data) - 1);
}

void history_adjust_pos(history *hist, int adjustment)
{
    if(!hist) return;

    history_set_pos(hist, hist->_history_index + adjustment);
}
