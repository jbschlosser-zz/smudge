#include <stdlib.h>
#include <string.h>
#include "mud_ui.h"

mud_ui *mud_ui_create(int output_buffer_size, int input_buffer_size, int history_size)
{
    if(output_buffer_size < 0) return NULL;
    if(input_buffer_size < 0) return NULL;
    if(history_size < 0) return NULL;

    mud_ui *ui = malloc(sizeof(mud_ui));
    ui->_output_window = mud_window_create(REAL_WINDOW_OPS, 0, 0, LINES - 1, COLS);
    ui->_output_buffer = scrollback_buffer_create(output_buffer_size);
    ui->_output_scrollback = 0;
    ui->_input_line = mud_window_create(REAL_WINDOW_OPS, LINES - 1, 0, 1, COLS);
    ui->_input_buffer = malloc(input_buffer_size);
    ui->_input_index = 0;
    ui->_max_input_size = input_buffer_size;
    mud_window_set_colors(ui->_input_line, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
    ui->_history = history_buffer_create(history_size);
    ui->_history_index = 0;

    return ui;
}

void mud_ui_destroy(mud_ui *ui)
{
    if(!ui) return;

    mud_window_destroy(ui->_output_window);
    scrollback_buffer_destroy(ui->_output_buffer);
    mud_window_destroy(ui->_input_line);
    free(ui->_input_buffer);
    history_buffer_destroy(ui->_history);
    free(ui);
}

int mud_ui_get_char(mud_ui *ui)
{
    return mud_window_get_char(ui->_input_line);
}

static void populate_window_with_scrollback(mud_window *mwin, scrollback_buffer *buf, int scrollback)
{
    int win_size_lines = mud_window_get_max_lines(mwin);
    int win_size_cols = mud_window_get_max_cols(mwin);

    // Scrolling all the way back is a special case.
    if(scrollback >= scrollback_buffer_num_lines(buf) - win_size_lines) {
        // Write lines starting at the first line in the scrollback buffer,
        // ending when the window is full.
        mud_window_clear(mwin);
        int i;
        int win_lines_written = 0;
        for(i = scrollback + win_size_lines - 1; i >= scrollback; --i) {
            mud_string *line = scrollback_buffer_get_line(buf, i);
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
        for(i = scrollback; i < scrollback + win_size_lines; ++i) {
            mud_string *line = scrollback_buffer_get_line(buf, i);
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
        for(j = i; j >= scrollback; --j) {
            mud_string *line = scrollback_buffer_get_line(buf, j);
            if(!line) continue;

            mud_window_write_text(mwin, line->_data + line_start_index, mud_string_length(line) - line_start_index);

            line_start_index = 0;
        }
    }
}

void mud_ui_resize(mud_ui *ui, int newlines, int newcols)
{
    if(!ui) return;
    if(newlines <= 0) return;
    if(newcols <= 0) return;

    // Resize the main window.
    mud_window_resize(ui->_output_window, newlines - 1, newcols);
    if((scrollback_buffer_num_lines(ui->_output_buffer) - ui->_output_scrollback + 1) < newlines)
        ui->_output_scrollback = scrollback_buffer_num_lines(ui->_output_buffer) - newlines + 1;
    populate_window_with_scrollback(ui->_output_window, ui->_output_buffer, ui->_output_scrollback);

    // Resize the input line.
    mud_window_resize(ui->_input_line, 1, newcols);
    mud_window_move(ui->_input_line, newlines - 1, 0);
    mud_window_set_colors(ui->_input_line, COLOR_PAIR(INPUT_LINE_COLOR_PAIR));
    mud_window_refresh(ui->_input_line);
}

void mud_ui_write_formatted_output(mud_ui *ui, mud_char_t *output, int len)
{
    if(!ui) return;
    if(!output) return;
    if(len < 0) return;

    // Write to the output buffer.
    scrollback_buffer_write(ui->_output_buffer, output, len);

    // Refresh the output window with the new data.
    populate_window_with_scrollback(ui->_output_window, ui->_output_buffer, ui->_output_scrollback);

    // Move the focus back to the input line.
    mud_window_refresh(ui->_input_line);
}

void mud_ui_write_unformatted_output(mud_ui *ui, const char *output, int len)
{
    if(!ui) return;
    if(!output) return;
    if(len < 0) return;
    
    // Convert to output to its formatted form.
    mud_char_t *formatted_form = malloc(len * sizeof(mud_char_t));
    int i;
    for(i = 0; i < len; ++i)
        formatted_form[i] = output[i];

    // Write the formatted form of the output.
    mud_ui_write_formatted_output(ui, formatted_form, len);

    free(formatted_form);
}

static void populate_window_with_buffer(mud_window *mwin, const char *buf, int len)
{
    mud_window_clear(mwin);
    // TODO: Fix this to write all at once.
    int i;
    for(i = 0; i < len; ++i) {
        mud_window_add_char(mwin, buf[i]);
    }
    mud_window_refresh(mwin);
}

static void mud_ui_set_input_line_from_history(mud_ui *ui)
{
    // Get the history entry at the history index.
    char *hist_entry = history_buffer_get_entry(ui->_history, ui->_history_index);

    // Copy the history entry into the input line.
    strncpy(ui->_input_buffer, hist_entry, strlen(hist_entry));
    ui->_input_index = strlen(hist_entry);

    // Refresh the input line.
    populate_window_with_buffer(ui->_input_line, ui->_input_buffer, ui->_input_index);
}

void mud_ui_history_back(mud_ui *ui)
{
    // Adjust the history index to move back.
    // TODO: This seems kind of backwards?
    ++ui->_history_index;
    if(ui->_history_index > history_buffer_used(ui->_history))
        ui->_history_index = history_buffer_used(ui->_history);

    mud_ui_set_input_line_from_history(ui);
}

void mud_ui_history_forward(mud_ui *ui)
{
    // Adjust the history index to move forward.
    // TODO: This seems kind of backwards?
    --ui->_history_index;
    if(ui->_history_index < 0)
        ui->_history_index = 0;
    if(ui->_history_index == 0) {
        mud_ui_history_forward_end(ui);
        return;
    }

    mud_ui_set_input_line_from_history(ui);
}

void mud_ui_history_forward_end(mud_ui *ui)
{
    // The forward end of history is a cleared input line.
    ui->_history_index = 0;
    ui->_input_index = 0;
    mud_window_clear(ui->_input_line);
}

void mud_ui_adjust_scrollback(mud_ui *ui, int adjustment)
{
    mud_ui_set_scrollback(ui, ui->_output_scrollback + adjustment);
}

void mud_ui_set_scrollback(mud_ui *ui, int scrollback)
{
    // TODO: The behavior at the back of scrollback needs to be thought out a little better.
    // It might be cool to have a dynamic stopping point where there are just enough lines to
    // fill the window. Currently, the max is dependent on the window size, and that doesn't
    // seem to make a lot of sense.
    int lines = mud_window_get_max_lines(ui->_output_window);
    int scroll_lines_avail = (scrollback_buffer_num_lines(ui->_output_buffer) - lines);
    if(scroll_lines_avail < 0)
        scroll_lines_avail = 0;
    if(scrollback > scroll_lines_avail)
        scrollback = scroll_lines_avail;
    if(scrollback < 0)
        scrollback = 0;
    ui->_output_scrollback = scrollback;

    // Refresh the output window with the new scroll settings.
    populate_window_with_scrollback(ui->_output_window, ui->_output_buffer, ui->_output_scrollback);

    // Move the focus back to the input line.
    mud_window_refresh(ui->_input_line);
}

int mud_ui_get_scrollback(mud_ui *ui)
{
    return ui->_output_scrollback;
}

int mud_ui_scrollback_avail(mud_ui *ui)
{
    return scrollback_buffer_num_lines(ui->_output_buffer);
}

void mud_ui_page_up(mud_ui *ui)
{
    // Figure out how much upward scrolling can be done.
    // The page up here is defined as half a screen's worth
    // of lines.
    int lines = mud_window_get_max_lines(ui->_output_window);
    mud_ui_adjust_scrollback(ui, (lines / 2) + 1);
}

void mud_ui_page_down(mud_ui *ui)
{
    // Figure out how much downward scrolling can be done.
    // The page down here is defined as half a screen's worth
    // of lines.
    int lines = mud_window_get_max_lines(ui->_output_window);
    mud_ui_adjust_scrollback(ui, -((lines / 2) + 1));
}

void mud_ui_input_add_char(mud_ui *ui, char ch)
{
    if(ui->_input_index < ui->_max_input_size) {
        // Add the char to the input line.
        ui->_input_buffer[ui->_input_index++] = ch;

        // Refresh the input line.
        populate_window_with_buffer(ui->_input_line, ui->_input_buffer, ui->_input_index);
    }
}

void mud_ui_input_delete_char(mud_ui *ui)
{
    // Adjust the input index to delete a char.
    --ui->_input_index;
    if(ui->_input_index < 0)
        ui->_input_index = 0;

    // Refresh the input line.
    populate_window_with_buffer(ui->_input_line, ui->_input_buffer, ui->_input_index);
}

char *mud_ui_submit_input(mud_ui *ui)
{
    // Add the input to the history.
    history_buffer_add_entry(ui->_history, ui->_input_buffer, ui->_input_index);

    // Echo the command locally in the output window. A LF character is
    // appended for correct formatting.
    ui->_input_buffer[ui->_input_index] = 10; // LF.
    mud_ui_write_unformatted_output(ui, ui->_input_buffer, ui->_input_index + 1); // +1 for the LF.

    // Copy the input so that it can be returned.
    char *input = strndup(ui->_input_buffer, ui->_input_index);

    // Start over in the input line. The history should always be at the end after submitting a command.
    mud_ui_history_forward_end(ui);

    return input;
}
