#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "utilities.h"
#include "scrollback_buffer.h"

scrollback_buffer *scrollback_buffer_create(int size_in_lines)
{
    if(size_in_lines <= 0) return NULL;

    /// TODO: add bailout logic for malloc calls resulting in NULL
    scrollback_buffer *buf = malloc(sizeof(scrollback_buffer));
    buf->_lines = malloc(sizeof(mud_string*) * size_in_lines);
    int i;
    for(i = 0; i < size_in_lines; ++i)
        buf->_lines[i] = mud_string_create_empty(256);
    buf->_size = size_in_lines;
    buf->_used = 0;
    buf->_write_offset = 0;

    return buf;
}

void scrollback_buffer_destroy(scrollback_buffer *buf)
{
    if(!buf) return;

    int i;
    for(i = 0; i < buf->_size; ++i)
        mud_string_destroy(buf->_lines[i]);
    free(buf->_lines);
    free(buf);
}

int scrollback_buffer_max_lines(scrollback_buffer *buf)
{
    if(!buf) return 0;

    return buf->_size;
}

int scrollback_buffer_num_lines(scrollback_buffer *buf)
{
    if(!buf) return 0;

    return buf->_used;
}

void scrollback_buffer_write(scrollback_buffer *buf, mud_char_t *source, const int len)
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

            // Update the count of lines in the buffer.
            ++buf->_used;
            if(buf->_used > buf->_size)
                buf->_used = buf->_size;

            // Check if a line will be overwritten.
            if(buf->_used == buf->_size) {
                // Clear the old line.
                mud_string_clear(buf->_lines[buf->_write_offset]);
            }
        }
    }
}

mud_string *scrollback_buffer_get_line(scrollback_buffer *buf, int lines_back)
{
    if(!buf) return NULL;
    if(lines_back < 0) return NULL;
    if(lines_back >= buf->_used) return NULL;

    int line_index = modulo(buf->_write_offset - lines_back, buf->_size);
    return buf->_lines[line_index];
}
