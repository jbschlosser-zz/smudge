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

#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <ncurses.h>
#include "input_line.h"
#include "scrollback.h"

void init_ncurses(void);
void end_ncurses(void);

// The user interface for the application. It handles displaying
// the data, resizing, and obtaining user input.
typedef struct {
    WINDOW *_output_window;
    WINDOW *_input_line_window;
} user_interface;

// Constructor/destructor.
user_interface *user_interface_create(int y_loc, int x_loc, int size_lines, int size_cols);
void user_interface_destroy(user_interface *ui);

// Member functions.
int user_interface_get_user_input(user_interface *ui);
void user_interface_resize(user_interface *ui, int newlines, int newcols);
void user_interface_refresh_output_window(user_interface *ui, scrollback *sb);
void user_interface_refresh_input_line_window(user_interface *ui, input_line *input);

#endif
