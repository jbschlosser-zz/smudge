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

#ifndef KEYS_H
#define KEYS_H

#include "session.h"
#include "user_interface.h"

// Typedef for a function to call when a key is pressed.
typedef void (*key_binding)(session *sess, user_interface *ui, int key);

// Entry in the key binding hash table.
typedef struct _table_entry {
    int keycode;
    key_binding binding;
    struct _table_entry *next; // Linked list of entries that correspond to this hash.
} _key_binding_table_entry;

// Key binding hash table.
typedef struct {
    int _buckets;
    _key_binding_table_entry **_bindings;
} key_binding_table;

// Construction/destruction.
key_binding_table *key_binding_table_create(void);
void key_binding_table_set_binding(key_binding_table *kb, int keycode, key_binding binding);
key_binding key_binding_table_get_binding(key_binding_table *kb, int keycode);
void key_binding_table_destroy(key_binding_table *kb);

// Key binding functions.
void key_binding_unset(session *sess, user_interface *ui, int key);
void key_binding_do_nothing(session *sess, user_interface *ui, int key);
void key_binding_page_up(session *sess, user_interface *ui, int key);
void key_binding_page_down(session *sess, user_interface *ui, int key);
void key_binding_history_back(session *sess, user_interface *ui, int key);
void key_binding_history_forward(session *sess, user_interface *ui, int key);
void key_binding_history_forward_end(session *sess, user_interface *ui, int key);
void key_binding_add_input_char(session *sess, user_interface *ui, int key);
void key_binding_delete_input_char(session *sess, user_interface *ui, int key);
void key_binding_backspace_input_char(session *sess, user_interface *ui, int key);
void key_binding_input_cursor_left(session *sess, user_interface *ui, int key);
void key_binding_input_cursor_right(session *sess, user_interface *ui, int key);
void key_binding_search(session *sess, user_interface *ui, int key);
void key_binding_submit_input(session *sess, user_interface *ui, int key);

#endif
