#ifndef MWINDOW_H
#define MWINDOW_H

#include "mud_char.h"
#include "window_ops.h"

typedef struct
{
    window_ops _wops;
    WINDOW *_win;
} mud_window;

// Construction/destruction.
mud_window *mud_window_create(window_ops wops, int y_loc, int x_loc, int lines, int cols);
void mud_window_destroy(mud_window *mwin);

// Member functions.
void mud_window_add_char(mud_window *mwin, mud_char_t ch);
void mud_window_write_text(mud_window *mwin, mud_char_t *text, int len);
void mud_window_clear(mud_window *mwin);
void mud_window_refresh(mud_window *main);
int mud_window_get_max_lines(mud_window *mwin);
int mud_window_get_max_cols(mud_window *mwin);
void mud_window_set_colors(mud_window *mwin, int color_pair);
void mud_window_resize(mud_window *mwin, int lines, int cols);
void mud_window_move(mud_window *mwin, int y_loc, int x_loc);
int mud_window_get_char(mud_window *mwin);
void mud_window_set_cursor(mud_window *mwin, int y, int x);

#endif
