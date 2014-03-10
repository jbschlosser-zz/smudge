#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include "input_line.h"
#include "mud_window.h"
#include "scrollback.h"

typedef struct {
    mud_window *_output_window;
    mud_window *_input_line_window;
} user_interface;

// Constructor/destructor.
user_interface *user_interface_create(window_ops output_wops, window_ops input_wops, int y_loc, int x_loc, int size_lines, int size_cols);
void user_interface_destroy(user_interface *ui);

// Member functions.
int user_interface_get_char(user_interface *ui);
void user_interface_resize(user_interface *ui, int newlines, int newcols);
void user_interface_refresh_output_window(user_interface *ui, scrollback *sb);
void user_interface_refresh_input_line_window(user_interface *ui, input_line *input);
//int user_interface_get_output_window_max_lines(user_interface *ui);
//int user_interface_get_output_window_max_cols(user_interface *ui);

#endif
