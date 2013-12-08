#ifndef MUD_STRING_H
#define MUD_STRING_H

#include "mud_char.h"

typedef struct {
    mud_char_t *_data; // The character data.
    int _max_size; // The max size of character data that can fit in the string.
    int _block_size; // The size of blocks allocated to hold characters.
    int _length; // The number of characters currently in the string.
} mud_string;

// Constructors/destructor.
mud_string *mud_string_create(int max_size, mud_char_t* initial_str, int len);
mud_string *mud_string_create_empty(int max_size);
mud_string *mud_string_create_from_c_string(int max_size, const char *initial_str);
void mud_string_destroy(mud_string *str);

// Member functions.
int mud_string_length(mud_string *str);
void mud_string_append(mud_string *str, const mud_char_t *append_str, int len);
void mud_string_append_c_str(mud_string *str, const char *append_str, int len);
void mud_string_assign(mud_string *str, mud_string *other_str);
void mud_string_clear(mud_string *str);
void mud_string_delete_char(mud_string *str);
char *mud_string_to_c_str(mud_string *str);
mud_char_t *mud_string_get_data(mud_string *str);

#endif
