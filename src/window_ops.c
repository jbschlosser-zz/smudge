#include "window_ops.h"

const window_ops REAL_WINDOW_OPS = {
    .newwin = newwin,
    .delwin = delwin,
    .waddch = waddch,
    .wrefresh = wrefresh,
    .wclear = wclear,
    .nodelay = nodelay,
    .keypad = keypad,
    .wmove = wmove
};
