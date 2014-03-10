#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <ncurses.h>
#include "input_line.h"
#include "scrollback.h"

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
