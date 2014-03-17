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

#include "search.h"

bool search_buffer(line_buffer *buf, int starting_line, const char* str, search_result *result)
{
    if(!buf) return false;
    if(starting_line < 0) return false;
    if(!str) return false;
    if(!result) return false;

    // Compile the regex.
    int err_offset = 0;
    const char *err_contents = NULL;
    pcre *compiled_regex = pcre_compile(str, /*PCRE_EXTENDED*/ 0, &err_contents, &err_offset, NULL);
    if(!compiled_regex) {
        // TODO: Handle the error!
        return false;
    }

    // Study the regex to decrease search time.
    pcre_extra *extra = pcre_study(compiled_regex, 0, &err_contents);
    if(err_contents) {
        pcre_free(compiled_regex);
        // TODO: Handle the error!
        return false;
    }

    // Set up the offsets vector to store the results.
    int offsets[99];

    int i;
    for(i = starting_line; i < line_buffer_num_lines(buf); ++i) {
        color_string *line_str = line_buffer_get_line_relative_to_current(buf, i);
        char *line = color_string_to_c_str(line_str);
        
        int res = pcre_exec(compiled_regex, extra, line, color_string_length(line_str), 0, 0, offsets, 99);
        if(res >= 0) {
            // Success!
            result->line_number = i;
            result->begin_index = offsets[0];
            result->end_index = offsets[1];
            free(line);
            pcre_free(compiled_regex);
            if(extra) {
                pcre_free(extra);
            }
            return true;
        }

        free(line);
    }

    pcre_free(compiled_regex);
    if(extra) {
        pcre_free(extra);
    }

    return false;
}
