#ifndef SCROLLBACK_H
#define SCROLLBACK_H

#include "line_buffer.h"
#include "search.h"

typedef struct {
    line_buffer *_data;
    int _scroll_index;
    bool _dirty;
} scrollback;

// Construction/destruction.
scrollback *scrollback_create(line_buffer *data);
void scrollback_destroy(scrollback *sb);

// Member functions.
void scrollback_write(scrollback *sb, color_char *data, int len);
int scrollback_get_scroll(scrollback *sb);
void scrollback_set_scroll(scrollback *sb, int index);
void scrollback_adjust_scroll(scrollback *sb, int adjustment);
void scrollback_search_backwards(scrollback *sb, int starting_line, const char* str, search_result *result);
bool scrollback_is_dirty(scrollback *sb);
void scrollback_clear_dirty(scrollback *sb);

#endif
