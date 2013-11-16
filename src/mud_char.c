#include "mud_char.h"
#include <regex.h>

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

// Note: This function expects a null-terminated string to be passed in.
mud_char_t get_char_attrs(char *escape_seq)
{
    if(!escape_seq) return -1;

    // Ensure that the regex has been compiled.
    static regex_t escape_seq_regex;
    static bool regex_initialized = false;
    if(!regex_initialized) {
        if(regcomp(&escape_seq_regex, "^\\[([0-9]+|[0-9]+;[0-9]+|[0-9]+;[0-9]+;[0-9]+)m$", REG_EXTENDED) != 0)
            return -1;
        regex_initialized = true;
    }

    // Check if the escape sequence matches the regex.
    int regex_result = regexec(&escape_seq_regex, escape_seq, 0, NULL, 0);
    if(regex_result == REG_NOMATCH)
        return -1;

    // Extract the information from the escape sequence. Three pieces of
    // information are potentially available: a character attribute (e.g.
    // bold, blink, etc.), a foreground color, and a background color.
    // The format for the escape sequence is '[{attr};{fg};{bg}m'.
    int attr = -1;
    int fg = -1;
    int bg = -1;
    mud_char_t char_attrs = 0;
    int available = sscanf(escape_seq, "[%d;%d;%dm", &attr, &fg, &bg);
    if(available < 1)
        return -1;
    if(available >= 1) {
        switch(attr) {
            case 0:
                // Normal.
                char_attrs = A_NORMAL;
                break;
            case 1:
                // Bright.
                char_attrs = A_BOLD;
                break;
            case 2:
                // Dim.
                char_attrs = A_DIM;
                break;
            case 3:
                // Underline.
                char_attrs = A_UNDERLINE;
                break;
            case 5:
                // Blink.
                char_attrs = A_BLINK;
                break;
            case 7:
                // Reverse.
                char_attrs = A_REVERSE;
                break;
            case 8:
                // Hidden.
                char_attrs = A_INVIS;
                break;
            default:
                return -1;
        }
    }
    if(available >= 2) {
        switch(fg) {
            case 30:
                char_attrs = char_attrs | COLOR_PAIR(BLACK_ON_DEFAULT_BG);
                break;
            case 31:
                char_attrs = char_attrs | COLOR_PAIR(RED_ON_DEFAULT_BG);
                break;
            case 32:
                char_attrs = char_attrs | COLOR_PAIR(GREEN_ON_DEFAULT_BG);
                break;
            case 33:
                char_attrs = char_attrs | COLOR_PAIR(YELLOW_ON_DEFAULT_BG);
                break;
            case 34:
                char_attrs = char_attrs | COLOR_PAIR(BLUE_ON_DEFAULT_BG);
                break;
            case 35:
                char_attrs = char_attrs | COLOR_PAIR(MAGENTA_ON_DEFAULT_BG);
                break;
            case 36:
                char_attrs = char_attrs | COLOR_PAIR(CYAN_ON_DEFAULT_BG);
                break;
            case 37:
                char_attrs = char_attrs | COLOR_PAIR(WHITE_ON_DEFAULT_BG);
                break;
            default:
                return -1;
        }
    }
    if(available == 3) {
        // Eh... don't bother with the background at this time.
        // It would require a bit of rework for handling colors,
        // and non-default backgrounds are ugly anyway.
    }

    return char_attrs;
}
