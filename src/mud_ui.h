#ifndef MUD_UI_H
#define MUD_UI_H

#include "line_buffer.h"
#include "mud_window.h"

typedef struct {
    mud_window *_output_window;
    mud_window *_input_line_window;
} mud_ui;

// Constructor/destructor.
mud_ui *mud_ui_create(window_ops output_wops, window_ops input_wops, int y_loc, int x_loc, int size_lines, int size_cols);
void mud_ui_destroy(mud_ui *ui);

// Member functions.
int mud_ui_get_char(mud_ui *ui);
void mud_ui_resize(mud_ui *ui, int newlines, int newcols);
void mud_ui_refresh_output_window(mud_ui *ui, line_buffer *output_data, int scrollback_index);
void mud_ui_refresh_input_line_window(mud_ui *ui, mud_string *input_data);
//int mud_ui_get_output_window_max_lines(mud_ui *ui);
//int mud_ui_get_output_window_max_cols(mud_ui *ui);

#endif
