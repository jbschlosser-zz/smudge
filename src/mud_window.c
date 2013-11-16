#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "mud_window.h"

mud_window *mud_window_create(window_ops wops, int y_loc, int x_loc, int lines, int cols)
{
    if(y_loc < 0 || x_loc < 0 || lines <= 0 || cols <= 0)
        return NULL;

    mud_window *mwin = malloc(sizeof(mud_window));
    memset(mwin, 0x00, sizeof(mud_window));
    mwin->_wops = wops;
    mwin->_win = wops.newwin(lines, cols, y_loc, x_loc);
    wops.nodelay(mwin->_win, TRUE); // Allows non-blocking checks for keypresses.
    wops.keypad(mwin->_win, TRUE); // Provide function keys as a single code.

    return mwin;
}

void mud_window_destroy(mud_window *mwin)
{
    if(!mwin) return;

    mwin->_wops.delwin(mwin->_win);
    free(mwin);
}

void mud_window_add_char(mud_window *mwin, mud_char_t ch)
{
    if(!mwin) return;

    waddch(mwin->_win, ch);
}

void mud_window_write_text(mud_window *mwin, mud_char_t *text, int len)
{
    if(!mwin) return;
    
    int i;
    mud_char_t char_to_add;

    for(i = 0; i < len; ++i) {
        // Skip null characters.
        if(text[i] == '\0') {
            continue;
        }

        // Add the character to the window to be displayed.
        char_to_add = text[i];
        mwin->_wops.waddch(mwin->_win, char_to_add);
    }

    mud_window_refresh(mwin);
}

void mud_window_clear(mud_window *mwin)
{
    if(!mwin) return;

    mwin->_wops.wclear(mwin->_win);
}

void mud_window_refresh(mud_window *mwin)
{
    if(!mwin) return;

    mwin->_wops.wrefresh(mwin->_win);
}

int mud_window_get_max_lines(mud_window *mwin)
{
    if(!mwin) return -1;

    int lines, cols;
    getmaxyx(mwin->_win, lines, cols);

    return lines + (cols * 0);
}

int mud_window_get_max_cols(mud_window *mwin)
{
    if(!mwin) return -1;

    int lines, cols;
    getmaxyx(mwin->_win, lines, cols);

    return cols + (lines * 0);
}

void mud_window_set_colors(mud_window *mwin, int color_pair)
{
    if(!mwin) return;
    if(color_pair < 0) return;

    wbkgd(mwin->_win, color_pair);
}

void mud_window_resize(mud_window *mwin, int lines, int cols)
{
    if(!mwin) return;
    if(lines <= 0) return;
    if(cols <= 0) return;

    wresize(mwin->_win, lines, cols);
}

void mud_window_move(mud_window *mwin, int y_loc, int x_loc)
{
    if(y_loc < 0) return;
    if(x_loc < 0) return;

    mvwin(mwin->_win, y_loc, x_loc);
}

int mud_window_get_char(mud_window *mwin)
{
    if(!mwin) return ERR;

    return wgetch(mwin->_win);
}
