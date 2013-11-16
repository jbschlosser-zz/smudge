#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "utilities.h"
#include "window_buffer.h"

window_buffer *window_buffer_create(int size, int win_lines, int win_cols)
{
    if(size <= 0) return NULL;
    if(win_cols <= 0) return NULL;
    if(win_lines <= 0) return NULL;

    /// TODO: add bailout logic for malloc calls resulting in NULL
    window_buffer *buf = malloc(sizeof(window_buffer));
    memset(buf, 0x00, sizeof(window_buffer));
    buf->_data = malloc(sizeof(mud_char_t) * size);
    buf->_swap_data = malloc(sizeof(mud_char_t) * size);
    buf->_size = size;
    buf->_win_lines = win_lines;
    buf->_win_cols = win_cols;
    buf->_end_offset = (buf->_size / win_cols) * win_cols;

    return buf;
}

void window_buffer_destroy(window_buffer *buf)
{
    if(!buf) return;

    free(buf->_data);
    free(buf->_swap_data);
    free(buf);
}

int window_buffer_size(window_buffer *buf)
{
    if(!buf) return 0;

    return buf->_size;
}

int window_buffer_used(window_buffer *buf)
{
    if(!buf) return 0;

    return buf->_used;
}

int window_buffer_num_lines(window_buffer *buf)
{
    int num_lines = (buf->_used / buf->_win_cols);
    if((buf->_used % buf->_win_cols) != 0)
        ++num_lines;

    return num_lines;
}

void window_buffer_write(window_buffer *buf, mud_char_t *source, const int len)
{
    if(!buf) return;
    if(!source) return;
    if(len <= 0) return;

    int i;
    for(i = 0; i < len; ++i) {
        // Write the character.
        buf->_data[buf->_write_offset] = source[i];

        // Adjust the amount in the buffer as necessary.
        ++buf->_used;
        if(buf->_used > buf->_end_offset)
            buf->_used = buf->_end_offset;

        // Move to the next character.
        buf->_write_offset = (buf->_write_offset + 1) % buf->_end_offset;

        // Check if blanks need to be inserted. They are inserted after
        // newlines to keep the distance constant between window lines
        // in the buffer.
        if((source[i] & 0xFF) == '\n') {
            // Fill in blanks for the rest of a window width.
            int num_blanks = buf->_win_cols - (buf->_write_offset % buf->_win_cols);
            if(num_blanks == buf->_win_cols)
                num_blanks = 0;
            int j;
            for(j = 0; j < num_blanks; ++j) {
                //mud_char_t blank = '\0';
                //window_buffer_write(buf, &blank, 1);

                //Write the character.
                buf->_data[buf->_write_offset] = '\0'; // '\0'

                // Adjust the amount in the buffer as necessary.
                ++buf->_used;
                if(buf->_used > buf->_end_offset)
                    buf->_used = buf->_end_offset;

                // Move to the next character.
                buf->_write_offset = (buf->_write_offset + 1) % buf->_end_offset;
            }
        }
    }
}

int window_buffer_read(window_buffer *buf, mud_char_t *dest, const int max_len, const int scrollback_lines)
{
    if(!buf) return -1;
    if(scrollback_lines < 0) return -1;
    if(!dest) return -1;
    if(max_len < 0) return -1;
    if(max_len == 0) return 0;

    // Determine how much to scroll back.
    int lines_to_scroll = (buf->_win_lines - 1) + scrollback_lines;
    int elems_to_scroll = (lines_to_scroll * buf->_win_cols) + (buf->_write_offset % buf->_win_cols);
    if(elems_to_scroll > buf->_used)
        elems_to_scroll = buf->_used;
    int read_offset = modulo((buf->_write_offset - elems_to_scroll), buf->_end_offset);

    // Read the data.
    int total_elems_read = 0;
    int total_elems_to_read = ((max_len < elems_to_scroll) ? max_len : elems_to_scroll);
    while(total_elems_read < total_elems_to_read) {
        int data_left = (buf->_end_offset - read_offset);
        int elems_left_to_read = (total_elems_to_read - total_elems_read);
        int elems_to_read = ((elems_left_to_read < data_left) ? elems_left_to_read : data_left);
        memcpy(dest + total_elems_read, buf->_data + read_offset, elems_to_read * sizeof(mud_char_t));
        total_elems_read += elems_to_read;
        read_offset = (read_offset + elems_to_read) % buf->_end_offset;
    }

    return total_elems_read;
}

void window_buffer_refit(window_buffer *buf, int win_lines, int win_cols)
{
    // Store details of the old data.
    int old_used = buf->_used;
    int old_write_offset = buf->_write_offset;
    int old_end_offset = buf->_end_offset;

    // Swap the data and the swap data.
    mud_char_t *temp = buf->_data;
    buf->_data = buf->_swap_data;
    buf->_swap_data = temp;

    // Set the buffer up for the new window dimensions.
    buf->_used = 0;
    buf->_write_offset = 0;
    buf->_win_cols = win_cols;
    buf->_win_lines = win_lines;
    buf->_end_offset = (buf->_size / win_cols) * win_cols;

    // Transfer the old data into the main data of the buffer, refitting it as necessary.
    int i;
    for(i = 0; i < old_used; ++i) {
        int old_data_index = modulo(old_write_offset - old_used + i, old_end_offset);
        if(buf->_swap_data[old_data_index] != '\0')
            window_buffer_write(buf, &buf->_swap_data[old_data_index], 1);
    }
}
