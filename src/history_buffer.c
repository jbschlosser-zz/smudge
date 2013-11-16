#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include "history_buffer.h"
#include "utilities.h"

history_buffer *history_buffer_create(int size)
{
    if(size <= 0) return NULL;

    /// TODO: add bailout logic for malloc calls resulting in NULL
    history_buffer *buf = malloc(sizeof(history_buffer));
    memset(buf, 0x00, sizeof(history_buffer));
    buf->_data = malloc(sizeof(char*) * size);
    buf->_size = size;

    return buf;
}

void history_buffer_destroy(history_buffer *buf)
{
    if(!buf) return;

    int i;
    for(i = 0; i < buf->_used; ++i)
        free(buf->_data[i]);
    free(buf->_data);
    free(buf);
}

int history_buffer_size(history_buffer *buf)
{
    if(!buf) return 0;

    return buf->_size;
}

int history_buffer_used(history_buffer *buf)
{
    if(!buf) return 0;

    return buf->_used;
}

void history_buffer_add_entry(history_buffer *buf, const char *entry, int len)
{
    if(!buf) return;
    if(!entry) return;
    if(len <= 0) return;

    if(buf->_used == buf->_size) {
        // The buffer is full. Adding the entry will result in an overwrite.
        // The overwritten entry needs to be freed.
        free(buf->_data[buf->_write_index]);
    }

    // Add the entry to the buffer.
    buf->_data[buf->_write_index] = strndup(entry, len);

    // Adjust buffer stats accordingly.
    buf->_write_index = (buf->_write_index + 1) % buf->_size;
    ++buf->_used;
    if(buf->_used > buf->_size)
        buf->_used = buf->_size;
}

char * history_buffer_get_entry(history_buffer *buf, int back_index)
{
    if(back_index <= 0) return NULL;
    if(back_index > buf->_used) return NULL;

    int buffer_index = modulo(buf->_write_index - back_index, buf->_size);
    return buf->_data[buffer_index];
}
