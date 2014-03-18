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
#include "input_line.h"

input_line *input_line_create(void)
{
    input_line *input = malloc(sizeof(input_line));
    input->_data = color_string_create_empty(256);
    input->_cursor_index = 0;
    input->_dirty = false;

    return input;
}

void input_line_destroy(input_line *input)
{
    if(!input) return;

    color_string_destroy(input->_data);
    free(input);
}

void input_line_add_char(input_line *input, color_char ch)
{
    if(!input) return;

    color_string_insert(input->_data, input_line_get_cursor(input), &ch, 1);
    ++input->_cursor_index;
    input->_dirty = true;
}

void input_line_delete_char(input_line *input)
{
    if(!input) return;
    if(color_string_length(input->_data) == 0) return;

    color_string_delete_char(input->_data, input->_cursor_index);
    input->_dirty = true;
}

void input_line_backspace_char(input_line *input)
{
    if(!input) return;
    if(color_string_length(input->_data) == 0) return;
    if(input->_cursor_index < 1) return;

    color_string_delete_char(input->_data, input->_cursor_index - 1);
    --input->_cursor_index;
    input->_dirty = true;
}

void input_line_clear(input_line *input)
{
    if(!input) return;

    color_string_clear(input->_data);
    input->_cursor_index = 0;
    input->_dirty = true;
}

color_string *input_line_get_contents(input_line *input)
{
    if(!input) return NULL;

    return input->_data;
}

void input_line_set_contents(input_line *input, color_string *str)
{
    if(!input) return;
    if(!str) return;

    // Clear the current input.
    input_line_clear(input);

    // Add the new input.
    int i;
    for(i = 0; i < color_string_length(str); ++i) {
        input_line_add_char(input, color_string_get_data(str)[i]);
    }
    input->_dirty = true;
}

int input_line_get_cursor(input_line *input)
{
    if(!input) return -1;

    return input->_cursor_index;
}

void input_line_set_cursor(input_line *input, int index)
{
    if(!input) return;

    input->_cursor_index = index;
    if(input->_cursor_index < 0) {
        input->_cursor_index = 0;
    }
    if(input->_cursor_index > color_string_length(input->_data)) {
        input->_cursor_index = color_string_length(input->_data);
    }
    input->_dirty = true;
}

void input_line_adjust_cursor(input_line *input, int adjustment)
{
    if(!input) return;

    input_line_set_cursor(input, input->_cursor_index + adjustment);
}

bool input_line_is_dirty(input_line *input)
{
    if(!input) return false;

    return input->_dirty;
}

void input_line_clear_dirty(input_line *input)
{
    if(!input) return;

    input->_dirty = false;
}
