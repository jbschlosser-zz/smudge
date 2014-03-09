#include "application.h"

// TODO: Remove the need for this include by properly abstracting away
// the scripting interface.
#include <libguile.h>

void application_add_input_char(mud_session *session, mud_ui *ui, mud_char_t ch)
{
    mud_string_append(session->input_data, &ch, 1);
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_delete_input_char(mud_session *session, mud_ui *ui)
{
    mud_string_delete_char(session->input_data);
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_submit_input(mud_session *session, mud_ui *ui)
{
    // Add the input to the history.
    application_history_add(session, ui, session->input_data);

    // Clear the input line.
    char *input = mud_string_to_c_str(session->input_data);
    mud_string_clear(session->input_data);

    // Write the input to the output window.
    application_write_output(session, ui, mud_string_get_data(session->input_data), mud_string_length(session->input_data));

    // Run the hook.
    SCM symbol = scm_c_lookup("send-command-hook");
    SCM send_command_hook = scm_variable_ref(symbol);
    if(scm_is_true(scm_hook_p(send_command_hook)) && scm_is_false(scm_hook_empty_p(send_command_hook))) {
        scm_c_run_hook(send_command_hook, scm_list_1(scm_from_locale_string(input)));
    } else {
        // Send the command to the server.
        int send_result = mud_connection_send_command(session->connection, input, strlen(input));
        if(send_result < 0) {
            // TODO: Handle this case!
            free(input);
            return;
        }
    }

    free(input);
}

void application_write_output(mud_session *session, mud_ui *ui, mud_char_t *output, int len)
{
    // Write the data, keeping the scrollback locked, if necessary.
    // TODO: Fix this so that it works when the scrollback buffer is full.
    // Currently, in this case, no adjustment will be made to lock scrollback
    // in the right place. Maybe write could return the number
    // of lines written?
    int lines_before = line_buffer_num_lines(session->scrollback_data);
    line_buffer_write(session->scrollback_data, output, len);
    int lines_after = line_buffer_num_lines(session->scrollback_data);
    if(mud_session_get_scrollback_index(session) > 0)
        mud_session_adjust_scrollback_index(session, lines_after - lines_before, mud_ui_get_output_window_max_lines(ui));

    // UPDATE THE OUTPUT WINDOW.
    mud_ui_populate_output_window(ui, session->scrollback_data, mud_session_get_scrollback_index(session));
    mud_ui_refresh(ui);
}

void application_history_back(mud_session *session, mud_ui *ui)
{
    mud_session_adjust_history_index(session, 1);
    mud_string_assign(session->input_data, line_buffer_get_line_relative_to_current(session->history_data, mud_session_get_history_index(session)));
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_history_forward(mud_session *session, mud_ui *ui)
{
    mud_session_adjust_history_index(session, -1);
    mud_string_assign(session->input_data, line_buffer_get_line_relative_to_current(session->history_data, mud_session_get_history_index(session)));
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_history_forward_end(mud_session *session, mud_ui *ui)
{
    mud_session_set_history_index(session, 0);
    mud_string_assign(session->input_data, line_buffer_get_line_relative_to_current(session->history_data, mud_session_get_history_index(session)));
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_history_add(mud_session *session, mud_ui *ui, mud_string *entry)
{
    line_buffer_write(session->history_data, mud_string_get_data(entry), mud_string_length(entry));
    mud_char_t newline = '\n';
    line_buffer_write(session->history_data, &newline, 1);
}

void application_page_up(mud_session *session, mud_ui *ui)
{
    mud_session_adjust_scrollback_index(session, 1, mud_ui_get_output_window_max_lines(ui));
    mud_ui_populate_output_window(ui, session->scrollback_data, mud_session_get_scrollback_index(session));
    mud_ui_refresh(ui);
}

void application_page_down(mud_session *session, mud_ui *ui)
{
    mud_session_adjust_scrollback_index(session, -1, mud_ui_get_output_window_max_lines(ui));
    mud_ui_populate_output_window(ui, session->scrollback_data, mud_session_get_scrollback_index(session));
    mud_ui_refresh(ui);
}

void application_resize(mud_session *session, mud_ui *ui, int lines, int cols)
{
    // Resize the UI.
    mud_ui_resize(ui, lines, cols);

    // Adjust the scrollback to account for the new window size. This
    // is necessary when the window is expanded to avoid unnecessary
    // blank space when scrolled all the way back.
    if((line_buffer_num_lines(session->scrollback_data) - mud_session_get_scrollback_index(session) + 1) < mud_ui_get_output_window_max_lines(ui))
        mud_session_set_scrollback_index(
            session,
            line_buffer_num_lines(session->scrollback_data) - mud_ui_get_output_window_max_lines(ui) + 1,
            mud_ui_get_output_window_max_lines(ui));

    // Update the UI.
    mud_ui_populate_output_window(ui, session->scrollback_data, mud_session_get_scrollback_index(session));
    mud_ui_populate_input_line_window(ui, session->input_data);
    mud_ui_refresh(ui);
}

void application_search_backwards(mud_session *session, mud_ui *ui, const char *search_str)
{
    // Check if a previous search has occurred.
    int search_start_line = 0;
    bool previous_match_present = (session->last_search_result.line_number != -1);
    if(previous_match_present) {
        // Indicate that the search should start from the location of the last result.
        search_start_line = session->last_search_result.line_number + 1;

        // Clear out any previous match.
        mud_char_t *previous_matching_line = mud_string_get_data(
                line_buffer_get_line_relative_to_current(
                    session->scrollback_data,
                    session->last_search_result.line_number));
        int j;
        for(j = session->last_search_result.begin_index; j < session->last_search_result.end_index; ++j) {
            // TODO: Make this safe.
            previous_matching_line[j] = previous_matching_line[j] ^ A_STANDOUT;
        }
    } else {
        // Indicate that the search should start from the current scrollback.
        search_start_line = mud_session_get_scrollback_index(session) + 1;
    }

    // Perform the search.
    search_result search;
    bool string_found = search_buffer(session->scrollback_data, search_start_line, search_str, &search);
    if(string_found) {
        // Move to and highlight the match.
        mud_session_set_scrollback_index(session, search.line_number, mud_ui_get_output_window_max_lines(ui));
        mud_char_t *line_with_result = mud_string_get_data(line_buffer_get_line_relative_to_current(session->scrollback_data, search.line_number));
        int j;
        for(j = search.begin_index; j < search.end_index; ++j)
            line_with_result[j] = line_with_result[j] | A_STANDOUT;

        // Save the match.
        session->last_search_result = search;
    } else {
        // No match was found.
        // TODO: Make this cleaner.
        session->last_search_result.line_number = -1;
        session->last_search_result.begin_index = -1;
        session->last_search_result.end_index = -1;
    }
}
