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

#ifndef HISTORY_H
#define HISTORY_H

#include "line_buffer.h"

// A history of commands sent by the user.
typedef struct {
    line_buffer *_data;
    int _history_index; // 0 corresponds to the current entry; positive numbers
                        // go back through the history.
} history;

// Construction/destruction.
history *history_create(line_buffer *data);
void history_destroy(history *hist);

// Member functions.
void history_add_entry(history *hist, color_string *entry);
color_string *history_get_current_entry(history *hist);
int history_get_pos(history *hist);
void history_set_pos(history *hist, int index);
void history_adjust_pos(history *hist, int adjustment);

#endif
