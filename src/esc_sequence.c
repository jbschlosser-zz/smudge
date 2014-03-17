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

#include <regex.h>
#include <stdlib.h>
#include "esc_sequence.h"

esc_sequence *esc_sequence_create(void)
{
    esc_sequence *seq = malloc(sizeof(esc_sequence));
    seq->len = 0;
    seq->ready = 0;

    return seq;
}

void esc_sequence_destroy(esc_sequence *seq)
{
    if(!seq) return;

    free(seq);
}

void esc_sequence_clear(esc_sequence *seq)
{
    if(!seq) return;

    seq->len = 0;
    seq->ready = 0;
}

int esc_sequence_update(esc_sequence *seq, char byte)
{
    if(!seq) return 0;

    if(seq->ready) {
        // Don't do anything until the sequence has been dealt with.
        return 0;
    }

    if(seq->len == 0) {
        // An escape character must be found before a sequence can begin.
        if(byte != ESCAPE_SEQUENCE_BEGIN) {
            return 0;
        }
        seq->data[seq->len++] = byte;
    } else {
        if(seq->len < ESCAPE_SEQUENCE_MAX_SIZE) {
            seq->data[seq->len++] = byte;
            if(byte == ESCAPE_SEQUENCE_END) {
                seq->data[seq->len] = '\0';
                seq->ready = 1;
            }
        } else {
            // Hit the max size... give up on this sequence.
            esc_sequence_clear(seq);
        }
    }

    return 1;
}

color_char esc_sequence_get_char_attrs(esc_sequence *seq)
{
    if(!seq) return -1;

    // Ensure that the regex has been compiled.
    static regex_t esc_sequence_regex;
    static bool regex_initialized = false;
    if(!regex_initialized) {
        if(regcomp(&esc_sequence_regex, "^\\[([0-9]+|[0-9]+;[0-9]+|[0-9]+;[0-9]+;[0-9]+)m$", REG_EXTENDED) != 0) {
            return -1;
        }
        regex_initialized = true;
    }

    // Check if the escape sequence matches the regex.
    int regex_result = regexec(&esc_sequence_regex, seq->data + 1 /* skip the ESC character */, 0, NULL, 0);
    if(regex_result == REG_NOMATCH) {
        return -1;
    }

    // Extract the information from the escape sequence. Three pieces of
    // information are potentially available: a character attribute (e.g.
    // bold, blink, etc.), a foreground color, and a background color.
    // The format for the escape sequence is '[{attr};{fg};{bg}m'.
    int attr = -1;
    int fg = -1;
    int bg = -1;
    color_char char_attrs = 0;
    int available = sscanf(seq->data + 1, "[%d;%d;%dm", &attr, &fg, &bg);
    if(available < 1) {
        return -1;
    }
    if(available >= 1) {
        switch(attr) {
            case 0:
                // Normal.
                char_attrs = A_NORMAL;
                break;
            case 1:
                // Bright.
                char_attrs = A_BOLD;
                break;
            case 2:
                // Dim.
                char_attrs = A_DIM;
                break;
            case 3:
                // Underline.
                char_attrs = A_UNDERLINE;
                break;
            case 5:
                // Blink.
                char_attrs = A_BLINK;
                break;
            case 7:
                // Reverse.
                char_attrs = A_REVERSE;
                break;
            case 8:
                // Hidden.
                char_attrs = A_INVIS;
                break;
            default:
                return -1;
        }
    }
    if(available >= 2) {
        switch(fg) {
            case 30:
                char_attrs = char_attrs | COLOR_PAIR(BLACK_ON_DEFAULT_BG);
                break;
            case 31:
                char_attrs = char_attrs | COLOR_PAIR(RED_ON_DEFAULT_BG);
                break;
            case 32:
                char_attrs = char_attrs | COLOR_PAIR(GREEN_ON_DEFAULT_BG);
                break;
            case 33:
                char_attrs = char_attrs | COLOR_PAIR(YELLOW_ON_DEFAULT_BG);
                break;
            case 34:
                char_attrs = char_attrs | COLOR_PAIR(BLUE_ON_DEFAULT_BG);
                break;
            case 35:
                char_attrs = char_attrs | COLOR_PAIR(MAGENTA_ON_DEFAULT_BG);
                break;
            case 36:
                char_attrs = char_attrs | COLOR_PAIR(CYAN_ON_DEFAULT_BG);
                break;
            case 37:
                char_attrs = char_attrs | COLOR_PAIR(WHITE_ON_DEFAULT_BG);
                break;
            default:
                return -1;
        }
    }
    if(available == 3) {
        // Eh... don't bother with the background at this time.
        // It would require a bit of rework for handling colors,
        // and non-default backgrounds are ugly anyway.
    }

    return char_attrs;
}
