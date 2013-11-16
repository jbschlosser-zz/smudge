#include "mock_window_ops.h"

WINDOW * mock_window_ops_do_nothing_newwin(int nlines, int ncols, int begin_y, int begin_x)
{
    WINDOW w;
    return &w;
}

int mock_window_ops_do_nothing_delwin(WINDOW *win)
{
    return 0;
}

int mock_window_ops_do_nothing_waddch(WINDOW *win, const chtype ch)
{
    return 0;
}

int mock_window_ops_do_nothing_wrefresh(WINDOW *win)
{
    return 0;
}

const window_ops DO_NOTHING_WINDOW_OPS = {
    .newwin = mock_window_ops_do_nothing_newwin,
    .delwin = mock_window_ops_do_nothing_delwin,
    .waddch = mock_window_ops_do_nothing_waddch,
    .wrefresh = mock_window_ops_do_nothing_wrefresh,
};
