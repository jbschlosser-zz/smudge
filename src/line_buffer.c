#include <stdlib.h>
#include "utilities.h"
#include "line_buffer.h"

line_buffer *line_buffer_create(int size_in_lines)
{
    if(size_in_lines <= 0) return NULL;

    /// TODO: add bailout logic for malloc calls resulting in NULL
    line_buffer *buf = malloc(sizeof(line_buffer));
    buf->_lines = malloc(sizeof(mud_string*) * size_in_lines);
    int i;
    for(i = 0; i < size_in_lines; ++i)
        buf->_lines[i] = mud_string_create_empty(128);
    buf->_size = size_in_lines;
    buf->_used = 1; // Count the first line as being in use.
    buf->_write_offset = 0;

    return buf;
}

void line_buffer_destroy(line_buffer *buf)
{
    if(!buf) return;

    int i;
    for(i = 0; i < buf->_size; ++i)
        mud_string_destroy(buf->_lines[i]);
    free(buf->_lines);
    free(buf);
}

int line_buffer_max_lines(line_buffer *buf)
{
    if(!buf) return 0;

    return buf->_size;
}

int line_buffer_num_lines(line_buffer *buf)
{
    if(!buf) return 0;

    return buf->_used;
}

void line_buffer_write(line_buffer *buf, const mud_char_t *source, const int len)
{
    if(!buf) return;
    if(!source) return;
    if(len <= 0) return;

    int i;
    for(i = 0; i < len; ++i) {
        // Write the character.
        mud_string_append(buf->_lines[buf->_write_offset], &source[i], 1);

        // Move to the next line if necessary.
        if((source[i] & 0xFF) == '\n') {
            buf->_write_offset = (buf->_write_offset + 1) % buf->_size;

            // Check if a line will be overwritten (i.e. the buffer is full).
            if(buf->_used == buf->_size) {
                // Clear the old line.
                mud_string_clear(buf->_lines[buf->_write_offset]);
            }

            // Update the count of lines in the buffer.
            ++buf->_used;
            if(buf->_used > buf->_size)
                buf->_used = buf->_size;
        }
    }
}

mud_string *line_buffer_get_line(line_buffer *buf, int index)
{
    if(!buf) return NULL;
    if(index < 0) return NULL;
    if(index >= buf->_used) return NULL;

    int line_index = modulo(buf->_write_offset - buf->_used + 1 + index, buf->_size);
    return buf->_lines[line_index];
}

mud_string *line_buffer_get_line_relative_to_current(line_buffer *buf, int lines_back)
{
    if(!buf) return NULL;
    if(lines_back < 0) return NULL;
    if(lines_back >= buf->_used) return NULL;

    int line_index = modulo(buf->_write_offset - lines_back, buf->_size);
    return buf->_lines[line_index];
}
