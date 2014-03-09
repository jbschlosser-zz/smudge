#include <stdlib.h>
#include "mud_ui.h"

mud_ui *mud_ui_create(window_ops output_wops, window_ops input_wops, int y_loc, int x_loc, int size_lines, int size_cols)
{
    if(y_loc < 0 || x_loc < 0 || size_lines <= 1 || size_cols <= 0)
        return NULL;

    mud_ui *ui = malloc(sizeof(mud_ui));
    ui->_output_window = mud_window_create(output_wops, y_loc, x_loc, size_lines - 1, size_cols);
    ui->_input_line_window = mud_window_create(input_wops, y_loc + size_lines - 1, x_loc, 1, size_cols);
    mud_window_set_colors(ui->_input_line_window, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));

    return ui;
}

void mud_ui_destroy(mud_ui *ui)
{
    mud_window_destroy(ui->_output_window);
    mud_window_destroy(ui->_input_line_window);
    free(ui);
}

int mud_ui_get_char(mud_ui *ui)
{
    return mud_window_get_char(ui->_input_line_window);
}

void mud_ui_resize(mud_ui *ui, int newlines, int newcols)
{
    if(!ui) return;
    if(newlines <= 0) return;
    if(newcols <= 0) return;

    // Resize the main window.
    mud_window_resize(ui->_output_window, newlines - 1, newcols);

    // Resize the input line.
    mud_window_resize(ui->_input_line_window, 1, newcols);
    mud_window_move(ui->_input_line_window, newlines - 1, 0);
    mud_window_set_colors(ui->_input_line_window, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
}

static void populate_window_with_scrollback(mud_window *mwin, line_buffer *output_data, int scrollback_index)
{
    int win_size_lines = mud_window_get_max_lines(mwin);
    int win_size_cols = mud_window_get_max_cols(mwin);

    // Scrolling all the way back is a special case.
    if(scrollback_index >= line_buffer_num_lines(output_data) - win_size_lines) {
        // Write lines starting at the first line in the scrollback buffer,
        // ending when the window is full.
        mud_window_clear(mwin);
        int i;
        int win_lines_written = 0;
        for(i = scrollback_index + win_size_lines - 1; i >= scrollback_index; --i) {
            mud_string *line = line_buffer_get_line_relative_to_current(output_data, i);
            if(!line) continue;

            // Write text from the line in increments of the number of columns
            // available in the window.
            int j;
            for(j = 0; j < mud_string_length(line); j += win_size_cols) {
                int line_left = (mud_string_length(line) - j);
                int amount_to_write = (win_size_cols < line_left ? win_size_cols : line_left);
                mud_window_write_text(mwin, line->_data + j, amount_to_write);
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
            mud_string *line = line_buffer_get_line_relative_to_current(output_data, i);
            if(!line) break;

            win_lines_found += (mud_string_length(line) / (win_size_cols + 1)) + 1;
            if(win_lines_found >= win_size_lines) {
                while(win_lines_found > win_size_lines) {
                    line_start_index += win_size_cols;
                    --win_lines_found;
                }
                break;
            }
        }

        // Write the lines.
        mud_window_clear(mwin);
        int j;
        for(j = i; j >= scrollback_index; --j) {
            mud_string *line = line_buffer_get_line_relative_to_current(output_data, j);
            if(!line) continue;

            mud_window_write_text(mwin, line->_data + line_start_index, mud_string_length(line) - line_start_index);

            line_start_index = 0;
        }
    }
}

void mud_ui_refresh_output_window(mud_ui *ui, scrollback *sb)
{
    // TODO: Fix this to use the public interface.
    populate_window_with_scrollback(ui->_output_window, sb->_data, scrollback_get_scroll(sb));
    mud_window_refresh(ui->_output_window);

    // Refresh the input line to keep the focus on it.
    mud_window_refresh(ui->_input_line_window);
}

void mud_ui_refresh_input_line_window(mud_ui *ui, input_line *input)
{
    mud_window_clear(ui->_input_line_window);
    mud_string *contents = input_line_get_contents(input);
    mud_window_write_text(ui->_input_line_window, mud_string_get_data(contents), mud_string_length(contents));
    mud_window_set_cursor(ui->_input_line_window, 0, input_line_get_cursor(input));
    mud_window_refresh(ui->_input_line_window);
}

/*int mud_ui_get_output_window_max_lines(mud_ui *ui)
{
    return mud_window_get_max_lines(ui->_output_window);
}

int mud_ui_get_output_window_max_cols(mud_ui *ui)
{
    return mud_window_get_max_cols(ui->_output_window);
}*/
