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
    if(y_loc < 0 || x_loc < 0 || size_lines <= 1 || size_cols <= 0)
        return NULL;

    user_interface *ui = malloc(sizeof(user_interface));
    ui->_output_window = newwin(size_lines - 1, size_cols, y_loc, x_loc);
    ui->_input_line_window = newwin(1, size_cols, y_loc + size_lines - 1, x_loc);

    // Set things up.
    wbkgd(ui->_input_line_window, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
    nodelay(ui->_input_line_window, TRUE); // Allows non-blocking checks for keypresses.
    keypad(ui->_input_line_window, TRUE); // Provide function keys as a single code.

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

static void populate_window_with_scrollback(WINDOW *win, line_buffer *output_data, int scrollback_index)
{
    int win_size_lines;
    int win_size_cols;
    getmaxyx(win, win_size_lines, win_size_cols);

    // TODO: This could be cleaned up a bit.

    // Scrolling all the way back is a special case.
    if(scrollback_index >= line_buffer_num_lines(output_data) - win_size_lines) {
        // Write lines starting at the first line in the scrollback buffer,
        // ending when the window is full.
        werase(win);
        int i;
        int win_lines_written = 0;
        for(i = scrollback_index + win_size_lines - 1; i >= scrollback_index; --i) {
            color_string *line = line_buffer_get_line_relative_to_current(output_data, i);
            if(!line) continue;

            // Write text from the line in increments of the number of columns
            // available in the window.
            int j;
            for(j = 0; j < color_string_length(line); j += win_size_cols) {
                int line_left = (color_string_length(line) - j);
                int amount_to_write = (win_size_cols < line_left ? win_size_cols : line_left);
                write_text_to_window(win, line->_data + j, amount_to_write);
                ++win_lines_written;
                if(win_lines_written == win_size_lines) break;
            }

            if(win_lines_written == win_size_lines) break;
        }
    } else {
        // Figure out which lines to write by counting window lines
        // backwards from the scrollback position.
        int win_lines_found = 0;
        int line_start_index = 0;
        int i;
        for(i = scrollback_index; i < scrollback_index + win_size_lines; ++i) {
            color_string *line = line_buffer_get_line_relative_to_current(output_data, i);
            if(!line) break;

            win_lines_found += (color_string_length(line) / (win_size_cols + 1)) + 1;
            if(win_lines_found >= win_size_lines) {
                while(win_lines_found > win_size_lines) {
                    line_start_index += win_size_cols;
                    --win_lines_found;
                }
                break;
            }
        }

        // Write the lines.
        werase(win);
        int j;
        for(j = i; j >= scrollback_index; --j) {
            color_string *line = line_buffer_get_line_relative_to_current(output_data, j);
            if(!line) continue;

            write_text_to_window(win, line->_data + line_start_index, color_string_length(line) - line_start_index);

            line_start_index = 0;
        }
    }
}

void user_interface_refresh_output_window(user_interface *ui, scrollback *sb)
{
    // TODO: Fix this to use the public interface.
    populate_window_with_scrollback(ui->_output_window, sb->_data, scrollback_get_scroll(sb));
    wrefresh(ui->_output_window);

    // Refresh the input line to keep the focus on it.
    wrefresh(ui->_input_line_window);
}

void user_interface_refresh_input_line_window(user_interface *ui, input_line *input)
{
    werase(ui->_input_line_window);
    color_string *contents = input_line_get_contents(input);
    write_text_to_window(ui->_input_line_window, color_string_get_data(contents), color_string_length(contents));
    wmove(ui->_input_line_window, 0, input_line_get_cursor(input));
    wrefresh(ui->_input_line_window);
}
