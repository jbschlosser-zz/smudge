#include "mud_char.h"

void init_color_pairs()
{
    init_pair(BLACK_ON_DEFAULT_BG, COLOR_BLACK, -1);
    init_pair(RED_ON_DEFAULT_BG, COLOR_RED, -1);
    init_pair(GREEN_ON_DEFAULT_BG, COLOR_GREEN, -1);
    init_pair(YELLOW_ON_DEFAULT_BG, COLOR_YELLOW, -1);
    init_pair(BLUE_ON_DEFAULT_BG, COLOR_BLUE, -1);
    init_pair(MAGENTA_ON_DEFAULT_BG, COLOR_MAGENTA, -1);
    init_pair(CYAN_ON_DEFAULT_BG, COLOR_CYAN, -1);
    init_pair(WHITE_ON_DEFAULT_BG, COLOR_WHITE, -1);
    init_pair(INPUT_LINE_COLOR_PAIR, COLOR_BLACK, COLOR_CYAN);
}
