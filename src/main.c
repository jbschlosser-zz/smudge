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

#include <ev.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "action.h"
#include "guile.h"
#include "keys.h"
#include "session.h"
#include "user_interface.h"

#define MAIN_BUFFER_MAX_SIZE 24576

int RESIZE_OCCURRED = 0;
void signal_handler(int signum)
{
    if(signum == SIGWINCH)
        RESIZE_OCCURRED = 1;
}

typedef struct {
    ev_io io;
    session *sess;
    user_interface *ui;
} io_with_state;

static void stdin_cb(EV_P_ struct ev_io *w, int revents)
{
    io_with_state *io_state = (io_with_state*)w;
    session *main_session = io_state->sess;
    user_interface *ui = io_state->ui;

    /*char buffer[2048];
    int byte_count = read(0, buffer, 2047);
    buffer[byte_count] = '\0';

    color_string *msg = color_string_create_from_c_str(buffer);
    action *act = (action*)write_output_line_action_create(msg);
    color_string_destroy(msg);
    act->perform(act, io_state->sess, io_state->ui);
    write_output_line_action_destroy(act);

    int scroll_index = user_interface_refresh_output_window(io_state->ui, io_state->sess->output_data);
    scrollback_set_scroll(io_state->sess->output_data, scroll_index);
    scrollback_clear_dirty(io_state->sess->output_data);*/

    // Perform the function bound to the key.
    int input_keycode = user_interface_get_user_input(ui);
    if(input_keycode != ERR) {
        action *act = key_binding_table_get_binding(main_session->bindings, input_keycode);
        if(act) {
            act->perform(act, main_session, ui);
        } else {
            // Indicate that the key is unbound.
            act = (action*)unset_key_binding_action_create(input_keycode);
            act->perform(act, main_session, ui);
            act->destroy(act);
        }

        user_interface_refresh_input_line_window(ui, main_session->input_data);
    }
}

void main_with_guile(void *data, int argc, char **argv)
{
    if(argc != 3 && data == NULL) {
        printf("Usage: mud <host> <port>\n");
        exit(1);
    }

    // SET UP SIGNAL HANDLERS.
    // Handle the window resize signal.
    struct sigaction signal_action;
    memset(&signal_action, 0x00, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGWINCH, &signal_action, NULL);

    // Purposely ignore SIGPIPE. When socket errors occur, they should
    // be checked for and handled at the location they occur.
    signal(SIGPIPE, SIG_IGN);

    // OPEN THE SYSLOG.
    openlog("mud", 0x00, LOG_USER);

    // SET UP GUILE.
    init_guile();

    // SET UP THE SESSION.
    session *main_session = session_create(
        mud_connection_create(REAL_SOCKET_OPS),
        scrollback_create(line_buffer_create(10000)),
        history_create(line_buffer_create(100)),
        input_line_create(),
        key_binding_table_create());
    set_guile_current_session(main_session);

    // CONNECT TO THE MUD SERVER.
    const char *host = argv[1];
    const char *port = argv[2];
    bool connected = mud_connection_connect(main_session->connection, host, port);
    if(!connected) {
        printf("Error: Could not make a connection to '%s' on port '%s'.\n", host, port);
        exit(1);
    }

    // SET UP THE USER INTERFACE.
    // Start ncurses.
    init_ncurses();
    nodelay(stdscr, TRUE); // Allows non-blocking checks for keypresses.
    keypad(stdscr, TRUE); // Provide function keys as a single code.

    // Create the UI.
    user_interface *ui = user_interface_create(0, 0, LINES, COLS);

    // MAIN LOOP (LIBEV VERSION).
    struct ev_loop *loop = ev_default_loop(0);
    io_with_state stdin_watcher;
    stdin_watcher.sess = main_session;
    stdin_watcher.ui = ui;
    ev_io_init(&(stdin_watcher.io), stdin_cb, 0, EV_READ);
    ev_io_start(loop, &(stdin_watcher.io));
    ev_loop(loop, 0);

    // MAIN LOOP.
    int input_keycode = 0x0;
    color_char received_data[MAIN_BUFFER_MAX_SIZE];
    while(1) {
        // Handle a resize if necessary.
        if(RESIZE_OCCURRED) {
            // Restart ncurses.
            endwin();
            refresh();
            clear();

            // Resize things.
            user_interface_resize(ui, LINES, COLS);

            // Refresh the UI.
            // TODO: Change this to use the public interface.
            main_session->output_data->_dirty = true;
            main_session->input_data->_dirty = true;

            // We're done here.
            RESIZE_OCCURRED = 0;
        }

        // GET DATA FROM THE SERVER.
        int received = mud_connection_receive(main_session->connection, received_data, MAIN_BUFFER_MAX_SIZE);
        bool connected = mud_connection_connected(main_session->connection);
        if(received < 0 || !connected) {
            // TODO: Handle this case!
            break;
        }
        if(received > 0) {
            // Add the data to scrollback.
            scrollback_write(main_session->output_data, received_data, received);
        }

        // REFRESH THE UI.
        if(scrollback_is_dirty(main_session->output_data)) {
            // Note that the scrollback index is adjusted based on the last line
            // written in the window. This helps cap the scrollback with the
            // window size.
            int scroll_index = user_interface_refresh_output_window(ui, main_session->output_data);
            scrollback_set_scroll(main_session->output_data, scroll_index);
            scrollback_clear_dirty(main_session->output_data);
        }
        if(input_line_is_dirty(main_session->input_data)) {
            user_interface_refresh_input_line_window(ui, main_session->input_data);
            input_line_clear_dirty(main_session->input_data);
        }

        // GET INPUT FROM THE USER.
    }

    end_ncurses();
    session_destroy(main_session);
    user_interface_destroy(ui);
}

int main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, &main_with_guile, NULL);

    // Never reached.
    return 0;
}
