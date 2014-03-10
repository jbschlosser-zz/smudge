#ifndef HISTORY_H
#define HISTORY_H

#include "line_buffer.h"

typedef struct {
    line_buffer *_data;
    int _history_index;
} history;

// Construction/destruction.
history *history_create(line_buffer *data);
void history_destroy(history *hist);

// Member functions.
void history_add_entry(history *hist, color_string *entry);
color_string *history_get_current_entry(history *hist);
int history_get_pos(history *hist);
void history_set_pos(history *hist, int index);
void history_adjust_pos(history *hist, int adjustment);

#endif
