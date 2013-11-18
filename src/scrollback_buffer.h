#ifndef SCROLLBACK_BUFFER_H
#define SCROLLBACK_BUFFER_H

#include "mud_string.h"

typedef struct {
    mud_string **_lines; // Contains the lines data.
    int _size; // The total size of the buffer in lines.
    int _used; // The number of lines in the buffer.
    int _write_offset; // Offset to the line to write to.
} scrollback_buffer;

// Creation/destruction.
scrollback_buffer *scrollback_buffer_create(int size_in_lines);
void scrollback_buffer_destroy(scrollback_buffer *buf);

// Member functions.
int scrollback_buffer_max_lines(scrollback_buffer *buf);
int scrollback_buffer_num_lines(scrollback_buffer *buf);
void scrollback_buffer_write(scrollback_buffer *buf, mud_char_t *source, const int len);
mud_string *scrollback_buffer_get_line(scrollback_buffer *buf, int line_num);

#endif
