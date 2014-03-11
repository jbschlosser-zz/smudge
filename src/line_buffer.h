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

#ifndef LINE_BUFFER_H
#define LINE_BUFFER_H

#include "color_string.h"

// Cirular buffer that operates on a line basis. Lines
// can be any length. When the buffer is full, oldest
// lines will be dumped to make way for new lines.
typedef struct {
    color_string **_lines; // Contains the lines data.
    int _size; // The total size of the buffer in lines.
    int _used; // The number of lines in the buffer, including the current one.
    int _write_offset; // Offset to the line to write to.
} line_buffer;

// Creation/destruction.
line_buffer *line_buffer_create(int size_in_lines);
void line_buffer_destroy(line_buffer *buf);

// Member functions.
int line_buffer_max_lines(line_buffer *buf);
int line_buffer_num_lines(line_buffer *buf);
void line_buffer_write(line_buffer *buf, const color_char *source, const int len);
color_string *line_buffer_get_line(line_buffer *buf, int index);
color_string *line_buffer_get_line_relative_to_current(line_buffer *buf, int lines_back);

#endif
