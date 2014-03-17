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

#include <stdlib.h>
#include <string.h>
#include "color_string.h"

void init_color_pairs()
{
    init_pair(BLACK_ON_DEFAULT_BG, COLOR_BLACK, -1);
    init_pair(RED_ON_DEFAULT_BG, COLOR_RED, -1);
    init_pair(GREEN_ON_DEFAULT_BG, COLOR_GREEN, -1);
    init_pair(YELLOW_ON_DEFAULT_BG, COLOR_YELLOW, -1);
    init_pair(BLUE_ON_DEFAULT_BG, COLOR_BLUE, -1);
    init_pair(MAGENTA_ON_DEFAULT_BG, COLOR_MAGENTA, -1);
    init_pair(CYAN_ON_DEFAULT_BG, COLOR_CYAN, -1);
    init_pair(WHITE_ON_DEFAULT_BG, COLOR_WHITE, -1);
    init_pair(INPUT_LINE_COLOR_PAIR, COLOR_BLACK, COLOR_CYAN);
}

color_string *color_string_create(int max_size, color_char* initial_str, int len)
{
    color_string *str = color_string_create_empty(max_size);
    if(!str) return NULL;
    color_string_append(str, initial_str, len);

    return str;
}

color_string *color_string_create_empty(int max_size)
{
    if(max_size <= 0) return NULL;

    color_string *str = malloc(sizeof(color_string));
    str->_data = malloc(sizeof(color_char) * max_size);
    str->_max_size = max_size;
    str->_block_size = max_size;
    str->_length = 0;

    return str;
}

color_string *color_string_create_from_c_string(int max_size, const char *initial_str)
{
    color_string *str = color_string_create_empty(max_size);
    if(!str) return NULL;
    color_string_append_c_str(str, initial_str, strlen(initial_str));

    return str;
}

void color_string_destroy(color_string *str)
{
    if(!str) return;

    free(str->_data);
    free(str);
}

int color_string_length(color_string *str)
{
    if(!str) return -1;

    return str->_length;
}

static color_char *convert_str(const char *str, int len)
{
    color_char *converted = malloc(sizeof(color_char) * len);
    int i;
    for(i = 0; i < len; ++i) {
        converted[i] = str[i];
    }

    return converted;
}

void color_string_append(color_string *str, const color_char *append_str, int len)
{
    if(!str) return;
    if(!append_str) return;
    if(len <= 0) return;

    // Make sure there is enough space to write the data.
    while((str->_max_size - str->_length) < len) {
        str->_max_size = (str->_max_size + str->_block_size);
        color_char *new_data = malloc(sizeof(color_char) * str->_max_size);
        memcpy(new_data, str->_data, str->_length * sizeof(color_char));
        free(str->_data);
        str->_data = new_data;
    }

    // Copy in the string to append.
    memcpy(str->_data + str->_length, append_str, len * sizeof(color_char));
    str->_length += len;
}

void color_string_append_c_str(color_string *str, const char *append_str, int len)
{
    if(!str) return;
    if(!append_str) return;
    if(len <= 0) return;

    color_char *converted = convert_str(append_str, len);
    color_string_append(str, converted, len);
    free(converted);
}

void color_string_insert(color_string *str, int index, const color_char *insert_str, int len)
{
    if(!str) return;
    if(index < 0 || index > color_string_length(str)) return;
    if(!insert_str) return;
    if(len <= 0) return;

    // Make sure there is enough space to write the data.
    while((str->_max_size - str->_length) < len) {
        str->_max_size = (str->_max_size + str->_block_size);
        color_char *new_data = malloc(sizeof(color_char) * str->_max_size);
        memcpy(new_data, str->_data, str->_length * sizeof(color_char));
        free(str->_data);
        str->_data = new_data;
    }

    // Move the data that is in the way.
    memmove(str->_data + index + len, str->_data + index, (str->_length - index) * sizeof(color_char));

    // Copy in the new data.
    memcpy(str->_data + index, insert_str, len * sizeof(color_char));

    // Indicate that the length has changed.
    str->_length += len;
}

void color_string_assign(color_string *str, color_string *other_str)
{
    if(!str) return;
    if(!other_str) return;

    // Check if the string will fit in the currently allocated memory.
    if(color_string_length(other_str) > color_string_length(str)) {
        // Allocate the same amount of space as the other string has.
        // TODO: Allocate in blocks.
        free(str->_data);
        str->_data = malloc(other_str->_max_size * sizeof(color_char));
        str->_block_size = other_str->_block_size;
        str->_max_size = other_str->_max_size;
    }

    // Copy over the data from the other string.
    memcpy(str->_data, other_str->_data, other_str->_length * sizeof(color_char));
    str->_length = other_str->_length;
}

void color_string_clear(color_string *str)
{
    if(!str) return;

    str->_length = 0;
}

void color_string_delete_char(color_string *str, int index)
{
    if(!str) return;
    if(index < 0) return;
    if(index >= color_string_length(str)) return;

    // Move down the characters after the deleted one.
    int i;
    for(i = index + 1; i < color_string_length(str); ++i) {
        str->_data[i - 1] = str->_data[i];
    }
    --str->_length;
}

char *color_string_to_c_str(color_string *str)
{
    if(!str) return NULL;

    char *c_str = malloc(str->_length + 1);
    int i;
    for(i = 0; i < str->_length; ++i) {
        c_str[i] = str->_data[i];
    }
    c_str[str->_length] = '\0';

    return c_str;
}

color_char *color_string_get_data(color_string *str)
{
    return str->_data;
}
