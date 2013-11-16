#ifndef WINDOW_BUFFER_H
#define WINDOW_BUFFER_H

#include "mud_char.h"

//typedef short mud_char_t;
typedef struct {
    mud_char_t *_data; // Contains the buffer data.
    mud_char_t *_swap_data; // Used as a temporary swap space when the window is resized.
    int _size; // The total size of the buffer in characters.
    int _win_lines; // The number of lines in the window.
    int _win_cols; // The number of columns in the window.
    int _used; // The number of elements in the buffer.
    int _write_offset; // Location to write to next in the buffer.
    int _end_offset; // Location to the end of the usable data. This changes based on the window size to keep usable data at a whole number of window widths.
} window_buffer;

// Creation/destruction.
window_buffer *window_buffer_create(int size, int win_lines, int win_cols);
void window_buffer_destroy(window_buffer *buf);

// Member functions.
int window_buffer_size(window_buffer *buf);
int window_buffer_used(window_buffer *buf);
int window_buffer_num_lines(window_buffer *buf);
void window_buffer_write(window_buffer *buf, mud_char_t *source, const int len);
int window_buffer_read(window_buffer *buf, mud_char_t *dest, const int max_len, const int scrollback_lines);
void window_buffer_refit(window_buffer *buf, int win_lines, int win_cols);

#endif
