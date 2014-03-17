/*
 * Copyright (C) 2014 Joel Schlosser All Rights Reserved.
 *
 * This file is part of smudge.
 *
 * smudge is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * smudge is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with smudge.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "scrollback.h"

scrollback *scrollback_create(line_buffer *data)
{
    if(!data) return NULL;

    scrollback *sb = malloc(sizeof(scrollback));
    sb->_data = data;
    sb->_scroll_index = 0;
    sb->_dirty = true;

    return sb;
}

void scrollback_destroy(scrollback *sb)
{
    if(!sb) return;

    line_buffer_destroy(sb->_data);
    free(sb);
}

void scrollback_write(scrollback *sb, color_char *data, int len)
{
    if(!sb) return;

    // Write the data, keeping the scrollback locked, if necessary.
    // TODO: Fix this so that it works when the scrollback buffer is full.
    // Currently, in this case, no adjustment will be made to lock scrollback
    // in the right place. Maybe write could return the number
    // of lines written?
    int lines_before = line_buffer_num_lines(sb->_data);
    line_buffer_write(sb->_data, data, len);
    int lines_after = line_buffer_num_lines(sb->_data);
    if(scrollback_get_scroll(sb) > 0) {
        scrollback_adjust_scroll(sb, lines_after - lines_before);
    }
    sb->_dirty = true;
}

int scrollback_get_scroll(scrollback *sb)
{
    if(!sb) return -1;

    return sb->_scroll_index;
}

void scrollback_set_scroll(scrollback *sb, int index)
{
    if(!sb) return;

    // TODO: Fix this so that it works properly with the window size.
    sb->_scroll_index = index;
    if(sb->_scroll_index < 0) {
        sb->_scroll_index = 0;
    }
    if(sb->_scroll_index >= line_buffer_num_lines(sb->_data)) {
        sb->_scroll_index = (line_buffer_num_lines(sb->_data) - 1);
    }
    sb->_dirty = true;
}

void scrollback_adjust_scroll(scrollback *sb, int adjustment)
{
    if(!sb) return;

    scrollback_set_scroll(sb, (sb->_scroll_index + adjustment));
}

void scrollback_search_backwards(scrollback *sb, const char* str, search_result *result)
{
    if(!sb) return;
    if(!result) return;

    // Check if a previous search has occurred.
    int search_start_line = 0;
    bool previous_match_present = (result->line_number != -1 /* TODO: Make sure the same string is being searched for. */);
    if(previous_match_present) {
        // Indicate that the search should start from the location of the last result.
        search_start_line = result->line_number + 1;

        // Clear out any previous match.
        color_char *previous_matching_line = color_string_get_data(
            line_buffer_get_line_relative_to_current(
                sb->_data,
                result->line_number));
        int j;
        for(j = result->begin_index; j < result->end_index; ++j) {
            // TODO: Make this safe.
            previous_matching_line[j] = previous_matching_line[j] ^ A_STANDOUT;
        }
    } else {
        // Indicate that the search should start from the current scrollback.
        search_start_line = scrollback_get_scroll(sb) + 1;
    }

    // Perform the search.
    bool string_found = search_buffer(sb->_data, search_start_line, str, result);
    if(string_found) {
        // Move to and highlight the match.
        scrollback_set_scroll(sb, result->line_number);
        color_char *line_with_result = color_string_get_data(line_buffer_get_line_relative_to_current(sb->_data, result->line_number));
        int j;
        for(j = result->begin_index; j < result->end_index; ++j) {
            line_with_result[j] = line_with_result[j] | A_STANDOUT;
        }
    } else {
        // No match was found.
        // TODO: Make this cleaner.
        result->line_number = -1;
        result->begin_index = -1;
        result->end_index = -1;
    }

    sb->_dirty = true;
}

bool scrollback_is_dirty(scrollback *sb)
{
    if(!sb) return false;

    return sb->_dirty;
}

void scrollback_clear_dirty(scrollback *sb)
{
    if(!sb) return;

    sb->_dirty = false;
}
