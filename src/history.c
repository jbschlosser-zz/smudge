#include <stdlib.h>
#include "history.h"

history *history_create(line_buffer *data)
{
    if(!data) return NULL;

    history *hist = malloc(sizeof(history));
    hist->_data = data;
    hist->_history_index = 0;

    return hist;
}

void history_destroy(history *hist)
{
    if(!hist) return;

    line_buffer_destroy(hist->_data);
    free(hist);
}

void history_add_entry(history *hist, mud_string *entry)
{
    if(!hist) return;
    if(!entry) return;

    line_buffer_write(hist->_data, mud_string_get_data(entry), mud_string_length(entry));
    mud_char_t newline = '\n';
    line_buffer_write(hist->_data, &newline, 1);

    // Adjust the index so that, if not pointing to the most recent entry, the current stays
    // the same.
    if(hist->_history_index != 0)
        ++hist->_history_index;
}

mud_string *history_get_current_entry(history *hist)
{
    if(!hist) return NULL;

    return line_buffer_get_line_relative_to_current(hist->_data, hist->_history_index);
}

int history_get_pos(history *hist)
{
    if(!hist) return -1;

    return hist->_history_index;
}

void history_set_pos(history *hist, int index)
{
    if(!hist) return;
    if(index < 0) return;

    hist->_history_index = index;
    if(hist->_history_index < 0)
        hist->_history_index = 0;
    if(hist->_history_index >= line_buffer_num_lines(hist->_data))
        hist->_history_index = (line_buffer_num_lines(hist->_data) - 1);
}

void history_adjust_pos(history *hist, int adjustment)
{
    if(!hist) return;

    history_set_pos(hist, hist->_history_index + adjustment);
}
