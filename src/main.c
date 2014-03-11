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

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
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

void main_with_guile(void *data, int argc, char **argv)
{
    if(argc != 3 && data == NULL) {
        printf("Usage: mud <host> <port>\n");
        exit(1);
    }

    // SET UP SIGNAL HANDLERS.
    // Handle the window resize signal.
    struct sigaction action;
    memset(&action, 0x00, sizeof(action));
    action.sa_handler = signal_handler;
    sigaction(SIGWINCH, &action, NULL);

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
        input_line_create());
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

    // Create the UI.
    user_interface *ui = user_interface_create(0, 0, LINES, COLS);

    // CREATE THE KEYBINDINGS.
    key_binding_table *bindings = key_binding_table_create();

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
            user_interface_refresh_output_window(ui, main_session->output_data);
            scrollback_clear_dirty(main_session->output_data);
        }
        if(input_line_is_dirty(main_session->input_data)) {
            user_interface_refresh_input_line_window(ui, main_session->input_data);
            input_line_clear_dirty(main_session->input_data);
        }

        // GET INPUT FROM THE USER.
        // Perform the function bound to the key.
        input_keycode = user_interface_get_user_input(ui);
        if(input_keycode == ERR) {
            usleep(10000);
        } else {
            key_binding binding = key_binding_table_get_binding(bindings, input_keycode);
            if(binding)
                binding(main_session, ui, input_keycode);
            else
                key_binding_unset(main_session, ui, input_keycode);
        }
    }

    end_ncurses();
    key_binding_table_destroy(bindings);
    session_destroy(main_session);
    user_interface_destroy(ui);
}

int main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, &main_with_guile, NULL);

    // Never reached.
    return 0;
}
