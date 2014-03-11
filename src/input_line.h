#ifndef INPUT_LINE_H
#define INPUT_LINE_H

#include "color_string.h"

// Input line for accepting user input.
typedef struct {
    color_string *_data;
    int _cursor_index; // 0 is the far left cursor position in the line; positive
                       // moves right.
    bool _dirty; // Indicates whether the input line has changed. Used to decide
                 // when the user interface should redraw the line.
} input_line;

// Construction/destruction.
input_line *input_line_create(void);
void input_line_destroy(input_line *input);

// Member functions.
void input_line_add_char(input_line *input, color_char ch);
void input_line_delete_char(input_line *input);
void input_line_backspace_char(input_line *input);
void input_line_clear(input_line *input);
color_string *input_line_get_contents(input_line *input);
void input_line_set_contents(input_line *input, color_string *str);
int input_line_get_cursor(input_line *input);
void input_line_set_cursor(input_line *input, int index);
void input_line_adjust_cursor(input_line *input, int adjustment);
bool input_line_is_dirty(input_line *input);
void input_line_clear_dirty(input_line *input);

#endif
