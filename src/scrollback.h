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

#ifndef SCROLLBACK_H
#define SCROLLBACK_H

#include "line_buffer.h"
#include "search.h"

// Stores data that can be scrolled back through.
typedef struct {
    line_buffer *_data;
    int _scroll_index; // 0 corresponds to the most recent line; positive moves
                       // backwards through the scrollback lines.
    bool _dirty; // Indicates whether the buffer has changed. Used to decide
                 // when the user interface should redraw the buffer.
} scrollback;

// Construction/destruction.
scrollback *scrollback_create(line_buffer *data);
void scrollback_destroy(scrollback *sb);

// Member functions.
void scrollback_write(scrollback *sb, color_char *data, int len);
int scrollback_get_scroll(scrollback *sb);
void scrollback_set_scroll(scrollback *sb, int index);
void scrollback_adjust_scroll(scrollback *sb, int adjustment);
void scrollback_search_backwards(scrollback *sb, const char* str, search_result *result);
bool scrollback_is_dirty(scrollback *sb);
void scrollback_clear_dirty(scrollback *sb);

#endif
