#include <stdlib.h>
#include <string.h>
#include "mud_string.h"

mud_string *mud_string_create(int max_size, mud_char_t* initial_str, int len)
{
    mud_string *str = mud_string_create_empty(max_size);
    if(!str) return NULL;
    mud_string_append(str, initial_str, len);

    return str;
}

mud_string *mud_string_create_empty(int max_size)
{
    if(max_size <= 0) return NULL;

    mud_string *str = malloc(sizeof(mud_string));
    str->_data = malloc(sizeof(mud_char_t) * max_size);
    str->_max_size = max_size;
    str->_block_size = max_size;
    str->_length = 0;

    return str;
}

mud_string *mud_string_create_from_c_string(int max_size, const char *initial_str)
{
    mud_string *str = mud_string_create_empty(max_size);
    if(!str) return NULL;
    mud_string_append_c_str(str, initial_str, strlen(initial_str));

    return str;
}

void mud_string_destroy(mud_string *str)
{
    if(!str) return;

    free(str->_data);
    free(str);
}

int mud_string_length(mud_string *str)
{
    if(!str) return -1;

    return str->_length;
}

static mud_char_t *convert_str(const char *str, int len)
{
    mud_char_t *converted = malloc(sizeof(mud_char_t) * len);
    int i;
    for(i = 0; i < len; ++i)
        converted[i] = str[i];

    return converted;
}

void mud_string_append(mud_string *str, const mud_char_t *append_str, int len)
{
    if(!str) return;
    if(!append_str) return;
    if(len <= 0) return;

    // Make sure there is enough space to write the data.
    while((str->_max_size - str->_length) < len) {
        str->_max_size = (str->_max_size + str->_block_size);
        mud_char_t *new_data = malloc(sizeof(mud_char_t) * str->_max_size);
        memcpy(new_data, str->_data, str->_length * sizeof(mud_char_t));
        free(str->_data);
        str->_data = new_data;
    }

    // Copy in the string to append.
    memcpy(str->_data + str->_length, append_str, len * sizeof(mud_char_t));
    str->_length += len;
}

void mud_string_append_c_str(mud_string *str, const char *append_str, int len)
{
    if(!str) return;
    if(!append_str) return;
    if(len <= 0) return;

    mud_char_t *converted = convert_str(append_str, len);
    mud_string_append(str, converted, len);
    free(converted);
}

void mud_string_assign(mud_string *str, mud_string *other_str)
{
    if(!str) return;
    if(!other_str) return;

    free(str->_data);
    str->_data = malloc(other_str->_max_size * sizeof(mud_char_t));
    memcpy(str->_data, other_str->_data, other_str->_length * sizeof(mud_char_t));
    str->_block_size = other_str->_block_size;
    str->_max_size = other_str->_max_size;
    str->_length = other_str->_length;
}

void mud_string_clear(mud_string *str)
{
    str->_length = 0;
}

void mud_string_delete_char(mud_string *str)
{
    --str->_length;
    if(str->_length < 0)
        str->_length = 0;
}

char *mud_string_to_c_str(mud_string *str)
{
    if(!str) return NULL;

    char *c_str = malloc(str->_length + 1);
    int i;
    for(i = 0; i < str->_length; ++i)
        c_str[i] = str->_data[i];
    c_str[str->_length] = '\0';

    return c_str;
}
