#ifndef SEARCH_H
#define SEARCH_H

#include <pcre.h>
#include "line_buffer.h"

// The result of a search.
typedef struct {
    int line_number;
    int begin_index;
    int end_index;
} search_result;

bool search_buffer(line_buffer *buf, int starting_line, const char* str, search_result *result);

#endif
