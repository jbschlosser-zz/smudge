#ifndef LINE_BUFFER_H
#define LINE_BUFFER_H

#include "mud_string.h"

typedef struct {
    mud_string **_lines; // Contains the lines data.
    int _size; // The total size of the buffer in lines.
    int _used; // The number of lines in the buffer, including the current one.
    int _write_offset; // Offset to the line to write to.
} line_buffer;

// Creation/destruction.
line_buffer *line_buffer_create(int size_in_lines);
void line_buffer_destroy(line_buffer *buf);

// Member functions.
int line_buffer_max_lines(line_buffer *buf);
int line_buffer_num_lines(line_buffer *buf);
void line_buffer_write(line_buffer *buf, const mud_char_t *source, const int len);
mud_string *line_buffer_get_line(line_buffer *buf, int index);
mud_string *line_buffer_get_line_relative_to_current(line_buffer *buf, int lines_back);

#endif
