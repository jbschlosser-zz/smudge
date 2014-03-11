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

#include <libguile.h>
#include <string.h>
#include <syslog.h>
#include "keys.h"

key_binding_table *key_binding_table_create(void)
{
    key_binding_table *kb_table = malloc(sizeof(key_binding_table));
    kb_table->_buckets = KEY_MAX;
    kb_table->_bindings = malloc(sizeof(_key_binding_table_entry*) * kb_table->_buckets);
    
    // Initialize the keybindings.
    int i;
    for(i = 0; i < kb_table->_buckets; ++i)
        kb_table->_bindings[i] = NULL;

    // Set the default keybindings.
    key_binding_table_set_binding(kb_table, 0xD, key_binding_do_nothing);
    key_binding_table_set_binding(kb_table, KEY_RESIZE, key_binding_do_nothing);
    key_binding_table_set_binding(kb_table, 27, key_binding_history_forward_end);
    key_binding_table_set_binding(kb_table, KEY_UP, key_binding_history_back);
    key_binding_table_set_binding(kb_table, KEY_DOWN, key_binding_history_forward);
    key_binding_table_set_binding(kb_table, 127 /* BACKSPACE. */, key_binding_backspace_input_char);
    key_binding_table_set_binding(kb_table, KEY_BACKSPACE, key_binding_backspace_input_char);
    key_binding_table_set_binding(kb_table, 330 /* DELETE. */, key_binding_delete_input_char);
    key_binding_table_set_binding(kb_table, KEY_PPAGE, key_binding_page_up);
    key_binding_table_set_binding(kb_table, KEY_NPAGE, key_binding_page_down);
    key_binding_table_set_binding(kb_table, KEY_HOME, key_binding_search);
    key_binding_table_set_binding(kb_table, 0xA, key_binding_submit_input);
    key_binding_table_set_binding(kb_table, KEY_LEFT, key_binding_input_cursor_left);
    key_binding_table_set_binding(kb_table, KEY_RIGHT, key_binding_input_cursor_right);
    for(i = 32; i<= 126; ++i)
        key_binding_table_set_binding(kb_table, i, key_binding_add_input_char);

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

void key_binding_table_set_binding(key_binding_table *kb_table, int keycode, key_binding binding)
{
    if(!kb_table) return;
    if(keycode < 0) return;
    if(!binding) return;

    // Check if an entry already exists.
    _key_binding_table_entry *current_entry = key_binding_table_get_entry(kb_table, keycode);
    if(current_entry) {
        // Set the binding for this key.
        current_entry->binding = binding;
    } else {
        // No entry with the hash exists; create one.
        _key_binding_table_entry *new_entry = malloc(sizeof(_key_binding_table_entry));
        new_entry->keycode = keycode;
        new_entry->binding = binding;
        int hash_value = key_binding_table_compute_hash(kb_table, keycode);
        new_entry->next = kb_table->_bindings[hash_value];

        // Add the new entry to the table.
        kb_table->_bindings[hash_value] = new_entry;
    }
}

key_binding key_binding_table_get_binding(key_binding_table *kb_table, int keycode)
{
    if(!kb_table) return NULL;
    if(keycode < 0) return NULL;

    // Get the key binding entry for the keycode, if one exists.
    _key_binding_table_entry *entry = key_binding_table_get_entry(kb_table, keycode);
    if(entry)
        return entry->binding;
    return NULL;
}

void key_binding_table_destroy(key_binding_table *kb_table)
{
    if(!kb_table) return;

    int i;
    for(i = 0; i < kb_table->_buckets; ++i) {
        _key_binding_table_entry *binding = kb_table->_bindings[i];
        while(binding) {
            _key_binding_table_entry *next_binding = binding->next;
            free(binding);
            binding = next_binding;
        }
    }
    free(kb_table->_bindings);
    free(kb_table);
}

void key_binding_unset(session *sess, user_interface *ui, int key)
{
    char msg[1024];
    sprintf(msg, "No binding set for key: %d\n", key);
    color_string *unset_msg = color_string_create_from_c_string(128, msg);
    scrollback_write(sess->output_data, color_string_get_data(unset_msg), color_string_length(unset_msg));
    color_string_destroy(unset_msg);
}

void key_binding_do_nothing(session *sess, user_interface *ui, int key)
{
    // Intentionally do nothing.
}

void key_binding_page_up(session *sess, user_interface *ui, int key)
{
    scrollback_adjust_scroll(sess->output_data, 1);
}

void key_binding_page_down(session *sess, user_interface *ui, int key)
{
    scrollback_adjust_scroll(sess->output_data, -1);
}

void key_binding_history_back(session *sess, user_interface *ui, int key)
{
    history_adjust_pos(sess->hist, 1);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
    syslog(LOG_DEBUG, "History back: Setting input line contents to: %s", history_get_current_entry(sess->hist));
}

void key_binding_history_forward(session *sess, user_interface *ui, int key)
{
    history_adjust_pos(sess->hist, -1);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
    syslog(LOG_DEBUG, "History forward: Setting input line contents to: %s", history_get_current_entry(sess->hist));
}

void key_binding_history_forward_end(session *sess, user_interface *ui, int key)
{
    history_set_pos(sess->hist, 0);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
}

void key_binding_add_input_char(session *sess, user_interface *ui, int key)
{
    color_char letter = key & 0xFF;
    input_line_add_char(sess->input_data, letter);
}

void key_binding_delete_input_char(session *sess, user_interface *ui, int key)
{
    input_line_delete_char(sess->input_data);
}

void key_binding_backspace_input_char(session *sess, user_interface *ui, int key)
{
    input_line_backspace_char(sess->input_data);
}

void key_binding_input_cursor_left(session *sess, user_interface *ui, int key)
{
    input_line_adjust_cursor(sess->input_data, -1);
}

void key_binding_input_cursor_right(session *sess, user_interface *ui, int key)
{
    input_line_adjust_cursor(sess->input_data, 1);
}

void key_binding_search(session *sess, user_interface *ui, int key)
{
    char *search_str = color_string_to_c_str(input_line_get_contents(sess->input_data));
    scrollback_search_backwards(sess->output_data, search_str, &sess->last_search_result);
    free(search_str);
}

void key_binding_submit_input(session *sess, user_interface *ui, int key)
{
    // Get the current input.
    color_string *current_input = input_line_get_contents(sess->input_data);

    // Add the input to the history.
    history_add_entry(sess->hist, current_input);
    history_set_pos(sess->hist, 0);

    // Run the hook if one is present.
    char *input_c_str = color_string_to_c_str(current_input);
    SCM symbol = scm_c_lookup("send-command-hook");
    SCM send_command_hook = scm_variable_ref(symbol);
    if(scm_is_true(scm_hook_p(send_command_hook)) && scm_is_false(scm_hook_empty_p(send_command_hook))) {
        scm_c_run_hook(send_command_hook, scm_list_1(scm_from_locale_string(input_c_str)));
    } else {
        // Write the input to the output window.
        scrollback_write(sess->output_data, color_string_get_data(current_input), color_string_length(current_input));
        color_char NEWLINE = '\n';
        scrollback_write(sess->output_data, &NEWLINE, 1);

        // Send the command to the server.
        int send_result = mud_connection_send_command(sess->connection, input_c_str, strlen(input_c_str));
        if(send_result < 0) {
            // TODO: Handle this case!
            free(input_c_str);
            return;
        }
    }

    // Clear the input line.
    input_line_clear(sess->input_data);
}
