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

#ifndef ESCAPE_SEQUENCE_H
#define ESCAPE_SEQUENCE_H

#include "color_string.h"

#define ESCAPE_SEQUENCE_MAX_SIZE 15
#define ESCAPE_SEQUENCE_BEGIN 0x1B // ESC character.
#define ESCAPE_SEQUENCE_END 0x6D // 'm' character.

// State machine for extracting ESC sequences. Once the beginning of
// a sequence is seen, subsequent data is stored until the end of
// the sequence or until an arbitrary max length. The ready flag
// will then be set to indicate that the ESC sequence should be
// handled externally.
typedef struct {
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
color_char esc_sequence_get_char_attrs(esc_sequence *seq);

#endif
