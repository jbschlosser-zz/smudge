#ifndef MOCK_WINDOW_OPS_H
#define MOCK_WINDOW_OPS_H

#include "../src/window_ops.h"

WINDOW * mock_window_ops_do_nothing_newwin(int nlines, int ncols, int begin_y, int begin_x);
int mock_window_ops_do_nothing_delwin(WINDOW *win);
int mock_window_ops_do_nothing_waddch(WINDOW *win, const chtype ch);
int mock_window_ops_do_nothing_wrefresh(WINDOW *win);

extern const window_ops DO_NOTHING_WINDOW_OPS;

#endif
