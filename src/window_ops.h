#ifndef WINDOW_OPS_H
#define WINDOW_OPS_H

#include <ncurses.h>

typedef struct
{
    WINDOW * (*newwin) (int nlines, int ncols, int begin_y, int begin_x);
    int (*delwin) (WINDOW *win);
    int (*waddch) (WINDOW *win, const chtype ch);
    int (*wrefresh) (WINDOW *win);
    int (*wclear) (WINDOW *win);
    int (*nodelay) (WINDOW *win, bool);
    int (*keypad) (WINDOW *win, bool);
} window_ops;

extern const window_ops REAL_WINDOW_OPS;

#endif
