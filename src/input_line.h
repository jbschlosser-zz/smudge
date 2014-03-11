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

#ifndef INPUT_LINE_H
#define INPUT_LINE_H

#include "color_string.h"

// Input line for accepting user input.
typedef struct {
    color_string *_data;
    int _cursor_index; // 0 is the far left cursor position in the line; positive
                       // moves right.
    bool _dirty; // Indicates whether the input line has changed. Used to decide
                 // when the user interface should redraw the line.
} input_line;

// Construction/destruction.
input_line *input_line_create(void);
void input_line_destroy(input_line *input);

// Member functions.
void input_line_add_char(input_line *input, color_char ch);
void input_line_delete_char(input_line *input);
void input_line_backspace_char(input_line *input);
void input_line_clear(input_line *input);
color_string *input_line_get_contents(input_line *input);
void input_line_set_contents(input_line *input, color_string *str);
int input_line_get_cursor(input_line *input);
void input_line_set_cursor(input_line *input, int index);
void input_line_adjust_cursor(input_line *input, int adjustment);
bool input_line_is_dirty(input_line *input);
void input_line_clear_dirty(input_line *input);

#endif
