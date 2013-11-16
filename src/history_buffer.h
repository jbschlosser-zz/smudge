#ifndef HISTORY_BUFFER_H
#define HISTORY_BUFFER_H

typedef struct {
    char **_data; // Contains the history data.
    int _size; // The number of entries that can be inserted into the buffer.
    int _used; // The number of history entries in use.
    int _write_index; // The index of the buffer to which to write the next entry.
} history_buffer;

// Creation/destruction.
history_buffer *history_buffer_create(int size);
void history_buffer_destroy(history_buffer *buf);

// Member functions.
int history_buffer_size(history_buffer *buf);
int history_buffer_used(history_buffer *buf);
void history_buffer_add_entry(history_buffer *buf, const char *entry, int len);
char * history_buffer_get_entry(history_buffer *buf, int back_index);

#endif
