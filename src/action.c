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
#include "action.h"

void action_default_destroy(action *act)
{
    if(!act) return;

    free(act);
}

DEFINE_ACTION(do_nothing_action, sess, ui)
{
    // Intentionally do nothing.
}

DEFINE_ACTION(page_up_action, sess, ui)
{
    int win_size_lines = user_interface_output_window_num_lines(ui);
    scrollback_adjust_scroll(sess->output_data, (win_size_lines / 2));
}

DEFINE_ACTION(page_down_action, sess, ui)
{
    int win_size_lines = user_interface_output_window_num_lines(ui);
    scrollback_adjust_scroll(sess->output_data, -(win_size_lines / 2));
}

DEFINE_ACTION(history_back_action, sess, ui)
{
    history_adjust_pos(sess->hist, 1);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
}

DEFINE_ACTION(history_forward_action, sess, ui)
{
    history_adjust_pos(sess->hist, -1);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
}

DEFINE_ACTION(history_forward_end_action, sess, ui)
{
    history_set_pos(sess->hist, 0);
    input_line_set_contents(sess->input_data, history_get_current_entry(sess->hist));
}

DEFINE_ACTION(delete_input_char_action, sess, ui)
{
    input_line_delete_char(sess->input_data);
}

DEFINE_ACTION(backspace_input_char_action, sess, ui)
{
    input_line_backspace_char(sess->input_data);
}

DEFINE_ACTION(clear_input_line_action, sess, ui)
{
    input_line_clear(sess->input_data);
}

DEFINE_ACTION(input_cursor_left_action, sess, ui)
{
    input_line_adjust_cursor(sess->input_data, -1);
}

DEFINE_ACTION(input_cursor_right_action, sess, ui)
{
    input_line_adjust_cursor(sess->input_data, 1);
}

DEFINE_ACTION(search_backwards_from_input_line_action, sess, ui)
{
    // Search backwards for the current contents of the input line.
    action *search_act = (action*)search_backwards_action_create(input_line_get_contents(sess->input_data));
    search_act->perform(search_act, sess, ui);
    search_act->destroy(search_act);
}

DEFINE_ACTION(submit_from_input_line_action, sess, ui)
{
    // Submit the current contents of the input line.
    action *submit_act = (action*)submit_input_action_create(input_line_get_contents(sess->input_data));
    submit_act->perform(submit_act, sess, ui);
    submit_act->destroy(submit_act);

    // Add the input to the history.
    action *history_add_act = (action*)add_to_history_action_create(input_line_get_contents(sess->input_data));
    history_add_act->perform(history_add_act, sess, ui);
    history_add_act->destroy(history_add_act);

    // Scroll the history to the end.
    action *history_scroll_act = (action*)history_forward_end_action_create();
    history_scroll_act->perform(history_scroll_act, sess, ui);
    history_scroll_act->destroy(history_scroll_act);
}

// ------------------------
// Search backwards action.
// ------------------------
DEFINE_ACTION_1_STATE_CREATE(search_backwards_action, derived, color_string *search_str,
    derived->search_str = color_string_copy(search_str);
);

DEFINE_ACTION_1_STATE_DESTROY(search_backwards_action, derived,
    color_string_destroy(derived->search_str);
);

DEFINE_ACTION_1_PERFORM(search_backwards_action, derived, sess, ui,
    // Search backwards through the scrollback data.
    char *search_c_str = color_string_to_c_str(derived->search_str);
    scrollback_search_backwards(sess->output_data, search_c_str, &sess->last_search_result);
    free(search_c_str);
);

// --------------------
// Send command action.
// --------------------
DEFINE_ACTION_1_STATE_CREATE(send_command_action, derived, char *command_str,
    // TODO: Make this safe.
    derived->command_str = strdup(command_str);
);

DEFINE_ACTION_1_STATE_DESTROY(send_command_action, derived,
    free(derived->command_str);
);

DEFINE_ACTION_1_PERFORM(send_command_action, derived, sess, ui,
    // Color the command text yellow to help distinguish it as input.
    color_string *cmd_to_write = color_string_create_from_c_str(derived->command_str);
    int i;
    for(i = 0; i < color_string_length(cmd_to_write); ++i) {
        color_string_get_data(cmd_to_write)[i] = (color_string_get_data(cmd_to_write)[i] | COLOR_PAIR(YELLOW_ON_DEFAULT_BG));
    }

    // Write the command to the output window.
    action *write_act = (action*)write_output_line_action_create(cmd_to_write);
    write_act->perform(write_act, sess, ui);
    write_act->destroy(write_act);
    color_string_destroy(cmd_to_write);

    // Send the command.
    int send_result = mud_connection_send_command(sess->connection, derived->command_str, strlen(derived->command_str));
    if(send_result < 0) {
        // TODO: Handle this case!
        return;
    }
);

// ----------------------
// Add input char action.
// ----------------------
DEFINE_ACTION_1_STATE_CREATE(add_input_char_action, derived, color_char char_to_add,
    derived->char_to_add = char_to_add;
);

DEFINE_ACTION_1_STATE_DESTROY(add_input_char_action, derived,
    // No state needs to be destroyed.
);

DEFINE_ACTION_1_PERFORM(add_input_char_action, derived, sess, ui,
    input_line_add_char(sess->input_data, derived->char_to_add);
);

// ----------------------
// Add to history action.
// ----------------------
DEFINE_ACTION_1_STATE_CREATE(add_to_history_action, derived, color_string *entry,
    derived->entry = color_string_copy(entry);
);

DEFINE_ACTION_1_STATE_DESTROY(add_to_history_action, derived,
    color_string_destroy(derived->entry);
);

DEFINE_ACTION_1_PERFORM(add_to_history_action, derived, sess, ui,
    history_add_entry(sess->hist, derived->entry);
);

// -------------------------
// Write output line action.
// -------------------------
DEFINE_ACTION_1_STATE_CREATE(write_output_line_action, derived, color_string *line,
    derived->line = color_string_copy(line);
);

DEFINE_ACTION_1_STATE_DESTROY(write_output_line_action, derived,
    color_string_destroy(derived->line);
);

DEFINE_ACTION_1_PERFORM(write_output_line_action, derived, sess, ui,
    // Write the line to the output window.
    scrollback_write(sess->output_data, color_string_get_data(derived->line), color_string_length(derived->line));
    color_char NEWLINE = '\n';
    scrollback_write(sess->output_data, &NEWLINE, 1);
);

// -------------------------
// Unset key binding action.
// -------------------------
DEFINE_ACTION_1_STATE_CREATE(unset_key_binding_action, derived, int keycode,
    derived->keycode = keycode;
);

DEFINE_ACTION_1_STATE_DESTROY(unset_key_binding_action, derived,
    // No state needs to be destroyed.
);

DEFINE_ACTION_1_PERFORM(unset_key_binding_action, derived, sess, ui,
    // Output a message indicating that the key is unbound.
    char msg[1024];
    sprintf(msg, "No binding set for key: %d", derived->keycode);
    color_string *msg_str = color_string_create_from_c_str(msg);
    action *message_act = (action*)write_output_line_action_create(msg_str);
    message_act->perform(message_act, sess, ui);
    message_act->destroy(message_act);
    color_string_destroy(msg_str);
);

// --------------------
// Submit input action.
// --------------------
DEFINE_ACTION_1_STATE_CREATE(submit_input_action, derived, color_string *input_str,
    derived->input_str = color_string_copy(input_str);
);

DEFINE_ACTION_1_STATE_DESTROY(submit_input_action, derived,
    color_string_destroy(derived->input_str);
);

DEFINE_ACTION_1_PERFORM(submit_input_action, derived, sess, ui,
    // Run the hook if one is present.
    char *input_c_str = color_string_to_c_str(derived->input_str);
    SCM symbol = scm_c_lookup("send-command-hook");
    SCM send_command_hook = scm_variable_ref(symbol);
    if(scm_is_true(scm_hook_p(send_command_hook)) && scm_is_false(scm_hook_empty_p(send_command_hook))) {
        scm_c_run_hook(send_command_hook, scm_list_1(scm_from_locale_string(input_c_str)));
    } else {
        // Send the command to the server.
        action *send_act = (action*)send_command_action_create(input_c_str);
        send_act->perform(send_act, sess, ui);
        send_act->destroy(send_act);
    }

    free(input_c_str);
);
