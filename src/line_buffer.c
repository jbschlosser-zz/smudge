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
#include "line_buffer.h"

line_buffer *line_buffer_create(int size_in_lines)
{
    if(size_in_lines <= 0) return NULL;

    /// TODO: add bailout logic for malloc calls resulting in NULL
    line_buffer *buf = malloc(sizeof(line_buffer));
    buf->_lines = malloc(sizeof(color_string*) * size_in_lines);
    int i;
    for(i = 0; i < size_in_lines; ++i)
        buf->_lines[i] = color_string_create_empty(128);
    buf->_size = size_in_lines;
    buf->_used = 1; // Count the first line as being in use.
    buf->_write_offset = 0;

    return buf;
}

void line_buffer_destroy(line_buffer *buf)
{
    if(!buf) return;

    int i;
    for(i = 0; i < buf->_size; ++i)
        color_string_destroy(buf->_lines[i]);
    free(buf->_lines);
    free(buf);
}

int line_buffer_max_lines(line_buffer *buf)
{
    if(!buf) return 0;

    return buf->_size;
}

int line_buffer_num_lines(line_buffer *buf)
{
    if(!buf) return 0;

    return buf->_used;
}

void line_buffer_write(line_buffer *buf, const color_char *source, const int len)
{
    if(!buf) return;
    if(!source) return;
    if(len <= 0) return;

    int i;
    for(i = 0; i < len; ++i) {
        // Write the character.
        color_string_append(buf->_lines[buf->_write_offset], &source[i], 1);

        // Move to the next line if necessary.
        if((source[i] & 0xFF) == '\n') {
            buf->_write_offset = (buf->_write_offset + 1) % buf->_size;

            // Check if a line will be overwritten (i.e. the buffer is full).
            if(buf->_used == buf->_size) {
                // Clear the old line.
                color_string_clear(buf->_lines[buf->_write_offset]);
            }

            // Update the count of lines in the buffer.
            ++buf->_used;
            if(buf->_used > buf->_size)
                buf->_used = buf->_size;
        }
    }
}

// Performs n % d in the mathematical way.
static int modulo(int n, int d)
{
    if(d < 0)
        d = -d;
    return ((n % d) + d) % d;
}

color_string *line_buffer_get_line(line_buffer *buf, int index)
{
    if(!buf) return NULL;
    if(index < 0) return NULL;
    if(index >= buf->_used) return NULL;

    int line_index = modulo(buf->_write_offset - buf->_used + 1 + index, buf->_size);
    return buf->_lines[line_index];
}

color_string *line_buffer_get_line_relative_to_current(line_buffer *buf, int lines_back)
{
    if(!buf) return NULL;
    if(lines_back < 0) return NULL;
    if(lines_back >= buf->_used) return NULL;

    int line_index = modulo(buf->_write_offset - lines_back, buf->_size);
    return buf->_lines[line_index];
}
