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
#include "user_interface.h"

void init_ncurses(void)
{
    initscr(); // Start ncurses mode.
    cbreak(); // Pass characters directly to the ui without waiting for the return key to be pressed.
    noecho(); // Don't echo characters as they are typed.
    start_color(); // Start colors.
    use_default_colors(); // Allows default colors (i.e. the usual color of the terminal foreground or background).
    init_color_pairs(); // Set up the color pairs to be used throughout.
}

void end_ncurses(void)
{
    endwin(); // End ncurses mode.
}

user_interface *user_interface_create(int y_loc, int x_loc, int size_lines, int size_cols)
{
    if(y_loc < 0 || x_loc < 0 || size_lines <= 1 || size_cols <= 0) {
        return NULL;
    }

    user_interface *ui = malloc(sizeof(user_interface));
    ui->_output_window = newwin(size_lines - 1, size_cols, y_loc, x_loc);
    ui->_input_line_window = newwin(1, size_cols, y_loc + size_lines - 1, x_loc);

    // Set things up.
    scrollok(ui->_output_window, TRUE);
    wbkgd(ui->_input_line_window, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
    wrefresh(ui->_input_line_window);
    //nodelay(ui->_input_line_window, TRUE); // Allows non-blocking checks for keypresses.
    //keypad(ui->_input_line_window, TRUE); // Provide function keys as a single code.

    return ui;
}

void user_interface_destroy(user_interface *ui)
{
    if(!ui) return;

    delwin(ui->_output_window);
    delwin(ui->_input_line_window);
    free(ui);
}

int user_interface_get_user_input(user_interface *ui)
{
    //int input = getch();
    //wrefresh(ui->_input_line_window);
    //return input;
    return wgetch(ui->_input_line_window);
}

void user_interface_resize(user_interface *ui, int newlines, int newcols)
{
    if(!ui) return;
    if(newlines <= 0) return;
    if(newcols <= 0) return;

    // Resize the main window.
    wresize(ui->_output_window, newlines - 1, newcols);

    // Resize the input line.
    wresize(ui->_input_line_window, 1, newcols);
    mvwin(ui->_input_line_window, newlines - 1, 0);
    wbkgd(ui->_input_line_window, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
}

static void write_text_to_window(WINDOW *win, color_char *text, int len)
{
    if(!win) return;
    if(!text) return;
    if(len <= 0) return;
    
    int i;
    for(i = 0; i < len; ++i) {
        waddch(win, text[i]);
    }
}

// Returns the scrollback index of the last line in the window. This helps cap the
// scrollback based on the window size.
static int populate_window_with_scrollback(WINDOW *win, line_buffer *output_data, int scrollback_index)
{
    // Write a window's worth of lines, ending with the line at the scrollback
    // position.
    int win_size_lines;
    int win_size_cols;
    getmaxyx(win, win_size_lines, win_size_cols);
    (void)win_size_cols;
    werase(win);
    int line_index;
    for(line_index = scrollback_index + win_size_lines - 1; line_index >= scrollback_index; --line_index) {
        color_string *line = line_buffer_get_line_relative_to_current(output_data, line_index);
        if(line) {
            write_text_to_window(win, color_string_get_data(line), color_string_length(line));
            if(line_index != scrollback_index) {
                // Don't add a newline at the end.. the scrollback index line should be at
                // the very bottom of the window.
                waddch(win, '\n');
            }
        }
    }

    // If the window isn't completely filled at this point,
    // keep writing lines until it is.
    int cursor_line;
    int cursor_col;
    getyx(win, cursor_line, cursor_col);
    (void)cursor_col;
    for (line_index = scrollback_index - 1;
        (line_index >= 0) && (cursor_line != win_size_lines - 1);
        --line_index, getyx(win, cursor_line, cursor_col))
    {
        waddch(win, '\n');
        color_string *line = line_buffer_get_line_relative_to_current(output_data, line_index);
        if(line) {
            write_text_to_window(win, color_string_get_data(line), color_string_length(line));
        }
    }

    // Return scrollback index of the last line that was written.
    return (line_index + 1);
}

// Returns the scrollback index of the last line in the window.
// TODO: There has to be a way to cap the scrollback to the
// window size that is more intuitive.
int user_interface_refresh_output_window(user_interface *ui, scrollback *sb)
{
    // TODO: Fix this to use the public interface.
    int scroll_index = populate_window_with_scrollback(ui->_output_window, sb->_data, scrollback_get_scroll(sb));
    wrefresh(ui->_output_window);

    // Refresh the input line to keep the focus on it.
    wrefresh(ui->_input_line_window);

    return scroll_index;
}

void user_interface_refresh_input_line_window(user_interface *ui, input_line *input)
{
    werase(ui->_input_line_window);
    color_string *contents = input_line_get_contents(input);
    write_text_to_window(ui->_input_line_window, color_string_get_data(contents), color_string_length(contents));
    wmove(ui->_input_line_window, 0, input_line_get_cursor(input));
    wrefresh(ui->_input_line_window);
}

int user_interface_output_window_num_lines(user_interface *ui)
{
    int win_size_lines;
    int win_size_cols;
    getmaxyx(ui->_output_window, win_size_lines, win_size_cols);
    (void)win_size_cols;

    return win_size_lines;
}
