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

#include "keys.h"

key_binding_table *key_binding_table_create(void)
{
    key_binding_table *kb_table = malloc(sizeof(key_binding_table));
    kb_table->_buckets = KEY_MAX;
    kb_table->_bindings = malloc(sizeof(_key_binding_table_entry*) * kb_table->_buckets);
    
    // Initialize the keybindings.
    int i;
    for(i = 0; i < kb_table->_buckets; ++i) {
        kb_table->_bindings[i] = NULL;
    }

    // Set the default keybindings.
    key_binding_table_set_binding(kb_table, 0xD, (action*)do_nothing_action_create());
    key_binding_table_set_binding(kb_table, KEY_RESIZE, (action*)do_nothing_action_create());
    key_binding_table_set_binding(kb_table, 27, (action*)history_forward_end_action_create());
    key_binding_table_set_binding(kb_table, KEY_UP, (action*)history_back_action_create());
    key_binding_table_set_binding(kb_table, KEY_DOWN, (action*)history_forward_action_create());
    key_binding_table_set_binding(kb_table, 127 /* BACKSPACE. */, (action*)backspace_input_char_action_create());
    key_binding_table_set_binding(kb_table, KEY_BACKSPACE, (action*)backspace_input_char_action_create());
    key_binding_table_set_binding(kb_table, 330 /* DELETE. */, (action*)delete_input_char_action_create());
    key_binding_table_set_binding(kb_table, KEY_PPAGE, (action*)page_up_action_create());
    key_binding_table_set_binding(kb_table, KEY_NPAGE, (action*)page_down_action_create());
    key_binding_table_set_binding(kb_table, KEY_HOME, (action*)search_backwards_from_input_line_action_create());
    key_binding_table_set_binding(kb_table, 0xA, (action*)submit_from_input_line_action_create());
    key_binding_table_set_binding(kb_table, KEY_LEFT, (action*)input_cursor_left_action_create());
    key_binding_table_set_binding(kb_table, KEY_RIGHT, (action*)input_cursor_right_action_create());
    for(i = 32; i<= 126; ++i) {
        action *act = (action*)add_input_char_action_create((color_char)i);
        key_binding_table_set_binding(kb_table, i, act);
    }

    return kb_table;
}

static int key_binding_table_compute_hash(key_binding_table *kb_table, int keycode)
{
    return (keycode % kb_table->_buckets);
}

static _key_binding_table_entry *key_binding_table_get_entry(key_binding_table *kb_table, int keycode)
{
    // Check if an entry with the hash exists.
    int hash_value = key_binding_table_compute_hash(kb_table, keycode);
    if(kb_table->_bindings[hash_value]) {
        // At least one entry with the hash exists; search through the
        // list to see if a binding is set for the keycode.
        _key_binding_table_entry *entry = kb_table->_bindings[hash_value];
        while(entry) {
            if(entry->keycode == keycode)
                return entry;
            entry = entry->next;
        }
    }

    return NULL;
}

void key_binding_table_set_binding(key_binding_table *kb_table, int keycode, action *action_to_bind)
{
    if(!kb_table) return;
    if(keycode < 0) return;
    if(!action_to_bind) return;

    // Check if an entry already exists.
    _key_binding_table_entry *current_entry = key_binding_table_get_entry(kb_table, keycode);
    if(current_entry) {
        // Set the binding for this key.
        current_entry->bound_action = action_to_bind;
    } else {
        // No entry with the hash exists; create one.
        _key_binding_table_entry *new_entry = malloc(sizeof(_key_binding_table_entry));
        new_entry->keycode = keycode;
        new_entry->bound_action = action_to_bind;
        int hash_value = key_binding_table_compute_hash(kb_table, keycode);
        new_entry->next = kb_table->_bindings[hash_value];

        // Add the new entry to the table.
        kb_table->_bindings[hash_value] = new_entry;
    }
}

action *key_binding_table_get_binding(key_binding_table *kb_table, int keycode)
{
    if(!kb_table) return NULL;
    if(keycode < 0) return NULL;

    // Get the key binding entry for the keycode, if one exists.
    _key_binding_table_entry *entry = key_binding_table_get_entry(kb_table, keycode);
    if(entry)
        return entry->bound_action;
    return NULL;
}

void key_binding_table_destroy(key_binding_table *kb_table)
{
    if(!kb_table) return;

    int i;
    for(i = 0; i < kb_table->_buckets; ++i) {
        _key_binding_table_entry *binding_entry = kb_table->_bindings[i];
        while(binding_entry) {
            _key_binding_table_entry *next_binding_entry = binding_entry->next;
            binding_entry->bound_action->destroy(binding_entry->bound_action);
            free(binding_entry);
            binding_entry = next_binding_entry;
        }
    }
    free(kb_table->_bindings);
    free(kb_table);
}
