#ifndef MUD_CHAR_H
#define MUD_CHAR_H

#include <ncurses.h>

#define BLACK_ON_DEFAULT_BG 1
#define RED_ON_DEFAULT_BG 2
#define GREEN_ON_DEFAULT_BG 3
#define YELLOW_ON_DEFAULT_BG 4
#define BLUE_ON_DEFAULT_BG 5
#define MAGENTA_ON_DEFAULT_BG 6
#define CYAN_ON_DEFAULT_BG 7
#define WHITE_ON_DEFAULT_BG 8
#define INPUT_LINE_COLOR_PAIR 9

typedef chtype mud_char_t;

void init_color_pairs();

#endif
