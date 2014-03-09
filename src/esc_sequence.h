#ifndef ESCAPE_SEQUENCE_H
#define ESCAPE_SEQUENCE_H

#include "mud_char.h"

#define ESCAPE_SEQUENCE_MAX_SIZE 15
#define ESCAPE_SEQUENCE_BEGIN 0x1B // ESC character.
#define ESCAPE_SEQUENCE_END 0x6D // 'm' character.

typedef struct
{
    char data[ESCAPE_SEQUENCE_MAX_SIZE + 1]; // +1 for NULL terminator.
    int len;
    int ready;
} esc_sequence;

// Constructor/destructor.
esc_sequence *esc_sequence_create(void);
void esc_sequence_destroy(esc_sequence *seq);

// Member functions.
void esc_sequence_clear(esc_sequence *seq);

// Returns 1 if the esc sequence state changed; 0 otherwise.
int esc_sequence_update(esc_sequence *seq, char byte);

// Extract character attributes (e.g. colors) from escape sequences.
mud_char_t esc_sequence_get_char_attrs(esc_sequence *seq);

#endif
