#include <libguile.h>
#include <string.h>
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

void key_binding_unset(mud_session *session, mud_ui *ui, int key)
{
    char msg[1024];
    sprintf(msg, "No binding set for key: %d\n", key);
    mud_string *unset_msg = mud_string_create_from_c_string(128, msg);
    scrollback_write(session->output_data, mud_string_get_data(unset_msg), mud_string_length(unset_msg));
    mud_string_destroy(unset_msg);
}

void key_binding_do_nothing(mud_session *session, mud_ui *ui, int key)
{
    // Intentionally do nothing.
}

void key_binding_page_up(mud_session *session, mud_ui *ui, int key)
{
    scrollback_adjust_scroll(session->output_data, 1);
}

void key_binding_page_down(mud_session *session, mud_ui *ui, int key)
{
    scrollback_adjust_scroll(session->output_data, -1);
}

void key_binding_history_back(mud_session *session, mud_ui *ui, int key)
{
    history_adjust_pos(session->hist, 1);
    input_line_set_contents(session->input_data, history_get_current_entry(session->hist));
}

void key_binding_history_forward(mud_session *session, mud_ui *ui, int key)
{
    history_adjust_pos(session->hist, -1);
    input_line_set_contents(session->input_data, history_get_current_entry(session->hist));
}

void key_binding_history_forward_end(mud_session *session, mud_ui *ui, int key)
{
    history_set_pos(session->hist, 0);
    input_line_set_contents(session->input_data, history_get_current_entry(session->hist));
}

void key_binding_add_input_char(mud_session *session, mud_ui *ui, int key)
{
    mud_char_t letter = key & 0xFF;
    input_line_add_char(session->input_data, letter);
}

void key_binding_delete_input_char(mud_session *session, mud_ui *ui, int key)
{
    input_line_delete_char(session->input_data);
}

void key_binding_backspace_input_char(mud_session *session, mud_ui *ui, int key)
{
    input_line_backspace_char(session->input_data);
}

void key_binding_input_cursor_left(mud_session *session, mud_ui *ui, int key)
{
    input_line_adjust_cursor(session->input_data, -1);
}

void key_binding_input_cursor_right(mud_session *session, mud_ui *ui, int key)
{
    input_line_adjust_cursor(session->input_data, 1);
}

void key_binding_search(mud_session *session, mud_ui *ui, int key)
{
    char *search_str = mud_string_to_c_str(input_line_get_contents(session->input_data));
    scrollback_search_backwards(session->output_data, scrollback_get_scroll(session->output_data) + 1, search_str, &session->last_search_result);
    free(search_str);
}

void key_binding_submit_input(mud_session *session, mud_ui *ui, int key)
{
    // Get the current input.
    mud_string *current_input = input_line_get_contents(session->input_data);

    // Add the input to the history.
    history_add_entry(session->hist, current_input);
    history_set_pos(session->hist, 0);

    // Write the input to the output window.
    scrollback_write(session->output_data, mud_string_get_data(current_input), mud_string_length(current_input));
    mud_char_t NEWLINE = '\n';
    scrollback_write(session->output_data, &NEWLINE, 1);

    // Run the hook.
    char *input_c_str = mud_string_to_c_str(current_input);
    SCM symbol = scm_c_lookup("send-command-hook");
    SCM send_command_hook = scm_variable_ref(symbol);
    if(scm_is_true(scm_hook_p(send_command_hook)) && scm_is_false(scm_hook_empty_p(send_command_hook))) {
        scm_c_run_hook(send_command_hook, scm_list_1(scm_from_locale_string(input_c_str)));
    } else {
        // Send the command to the server.
        int send_result = mud_connection_send_command(session->connection, input_c_str, strlen(input_c_str));
        if(send_result < 0) {
            // TODO: Handle this case!
            free(input_c_str);
            return;
        }
    }

    // Clear the input line.
    input_line_clear(session->input_data);
}
