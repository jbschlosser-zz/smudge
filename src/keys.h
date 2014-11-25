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

// Forward declaration to resolve circular dependency.
struct _action;

// Entry in the key binding hash table.
typedef struct _table_entry {
    int keycode;
    struct _action *bound_action;
    struct _table_entry *next; // Linked list of entries that correspond to this hash.
} _key_binding_table_entry;

// Key binding hash table. Keys are bound to actions to make things happen.
typedef struct _key_binding_table {
    int _buckets;
    _key_binding_table_entry **_bindings;
} key_binding_table;

// Construction/destruction.
key_binding_table *key_binding_table_create(void);
void key_binding_table_set_binding(key_binding_table *kb, int keycode, struct _action *action_to_bind);
struct _action *key_binding_table_get_binding(key_binding_table *kb, int keycode);
void key_binding_table_destroy(key_binding_table *kb);

#endif
