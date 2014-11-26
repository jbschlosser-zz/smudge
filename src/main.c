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

typedef struct {
    ev_io io;
    session *sess;
    user_interface *ui;
    color_char received_data[MAIN_BUFFER_MAX_SIZE];
} io_with_state;

typedef struct {
    ev_signal signal;
    session *sess;
    user_interface *ui;
} signal_with_state;

static void user_input_cb(EV_P_ struct ev_io *w, int revents)
{
    io_with_state *io_state = (io_with_state*)w;
    session *main_session = io_state->sess;
    user_interface *ui = io_state->ui;

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

        // Refresh the UI.
        int scroll_index = user_interface_refresh_output_window(ui, main_session->output_data);
        scrollback_set_scroll(main_session->output_data, scroll_index);
        user_interface_refresh_input_line_window(ui, main_session->input_data);
    }
}

static void resize_cb(struct ev_loop *loop, struct ev_signal *w, int revents)
{
    signal_with_state *signal_state = (signal_with_state*)w;
    session *main_session = signal_state->sess;
    user_interface *ui = signal_state->ui;

    // Restart ncurses.
    endwin();
    refresh();
    clear();

    // Resize things.
    user_interface_resize(ui, LINES, COLS);

    // Refresh the output window.
    // Note that the scrollback index is adjusted based on the last line
    // written in the window. This helps cap the scrollback with the
    // window size.
    int scroll_index = user_interface_refresh_output_window(ui, main_session->output_data);
    scrollback_set_scroll(main_session->output_data, scroll_index);

    // Refresh the input line.
    user_interface_refresh_input_line_window(ui, main_session->input_data);
}

static void server_data_cb(EV_P_ struct ev_io *w, int revents)
{
    io_with_state *io_state = (io_with_state*)w;
    session *main_session = io_state->sess;
    user_interface *ui = io_state->ui;
    color_char *received_data = io_state->received_data;

    int received = mud_connection_receive(main_session->connection, received_data, MAIN_BUFFER_MAX_SIZE);
    bool connected = mud_connection_connected(main_session->connection);
    if(received < 0 || !connected) {
        // TODO: Handle this case!
        ev_io_stop(EV_A_ w);
        ev_unloop(EV_A_ EVUNLOOP_ALL);
    }
    if(received > 0) {
        // Add the data to scrollback.
        scrollback_write(main_session->output_data, received_data, received);

        // Refresh the output window.
        int scroll_index = user_interface_refresh_output_window(ui, main_session->output_data);
        scrollback_set_scroll(main_session->output_data, scroll_index);
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
    /*struct sigaction signal_action;
    memset(&signal_action, 0x00, sizeof(signal_action));
    signal_action.sa_handler = signal_handler;
    sigaction(SIGWINCH, &signal_action, NULL);*/

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

    // Create the UI.
    user_interface *ui = user_interface_create(0, 0, LINES, COLS);

    // SETUP MAIN LOOP.
    struct ev_loop *loop = ev_default_loop(0);

    // User input watcher.
    io_with_state stdin_watcher;
    stdin_watcher.sess = main_session;
    stdin_watcher.ui = ui;
    ev_io_init(&stdin_watcher.io, user_input_cb, 0 /* STDIN FD */, EV_READ);
    ev_io_start(loop, &stdin_watcher.io);

    // Resize signal watcher.
    signal_with_state resize_watcher;
    resize_watcher.sess = main_session;
    resize_watcher.ui = ui;
    ev_signal_init(&resize_watcher.signal, resize_cb, SIGWINCH);
    ev_signal_start(loop, &resize_watcher.signal);

    // MUD server data received watcher. (will be added on the fly for multiple sessions).
    io_with_state connection_watcher;
    connection_watcher.sess = main_session;
    connection_watcher.ui = ui;
    ev_io_init(&connection_watcher.io, server_data_cb,
        main_session->connection->_fd, EV_READ);
    ev_io_start(loop, &connection_watcher.io);

    // MAIN LOOP.
    ev_loop(loop, 0);

    // CLEAN UP.
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
