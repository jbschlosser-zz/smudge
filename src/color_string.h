#ifndef COLOR_STRING_H
#define COLOR_STRING_H

#include <ncurses.h>

// Typedef for a single colored character. It is currently
// coupled to ncurses, which isn't ideal...
typedef chtype color_char;

#define BLACK_ON_DEFAULT_BG 1
#define RED_ON_DEFAULT_BG 2
#define GREEN_ON_DEFAULT_BG 3
#define YELLOW_ON_DEFAULT_BG 4
#define BLUE_ON_DEFAULT_BG 5
#define MAGENTA_ON_DEFAULT_BG 6
#define CYAN_ON_DEFAULT_BG 7
#define WHITE_ON_DEFAULT_BG 8
#define INPUT_LINE_COLOR_PAIR 9

// Sets up the color pairs as listed above.
void init_color_pairs(void);

// A string of colored characters. Strings are automatically resized
// when necessary.
typedef struct {
    color_char *_data; // The character data.
    int _max_size; // The max size of character data that can fit in the string.
    int _block_size; // The size of blocks allocated to hold characters. TODO: Implement
                     // growth by some factor (e.g. 1.5 or 2) of current size instead.
    int _length; // The number of characters currently in the string.
} color_string;

// Constructors/destructor.
color_string *color_string_create(int max_size, color_char* initial_str, int len);
color_string *color_string_create_empty(int max_size);
color_string *color_string_create_from_c_string(int max_size, const char *initial_str);
void color_string_destroy(color_string *str);

// Member functions.
int color_string_length(color_string *str);
void color_string_append(color_string *str, const color_char *append_str, int len);
void color_string_append_c_str(color_string *str, const char *append_str, int len);
void color_string_insert(color_string *str, int index, const color_char *insert_str, int len);
void color_string_assign(color_string *str, color_string *other_str);
void color_string_clear(color_string *str);
void color_string_delete_char(color_string *str, int index);
char *color_string_to_c_str(color_string *str);
color_char *color_string_get_data(color_string *str);

#endif
