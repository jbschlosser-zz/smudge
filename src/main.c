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
    session *active_session;
    user_interface *ui;
} app_state;

typedef struct {
    ev_io io;
    app_state *state;
} io_with_state;

typedef struct {
    ev_io io;
    app_state *state;
    color_char recv_buffer[MAIN_BUFFER_MAX_SIZE];
    session *sess;
} connection_io;

typedef struct {
    ev_signal signal;
    app_state *state;
} signal_with_state;

static void user_input_cb(EV_P_ struct ev_io *w, int revents)
{
    app_state *state = ((io_with_state*)w)->state;

    // Perform the function bound to the key.
    int input_keycode = user_interface_get_user_input(state->ui);
    if(input_keycode != ERR) {
        action *act = key_binding_table_get_binding(state->active_session->bindings, input_keycode);
        if(act) {
            act->perform(act, state->active_session, state->ui);
        } else {
            // Indicate that the key is unbound.
            act = (action*)unset_key_binding_action_create(input_keycode);
            act->perform(act, state->active_session, state->ui);
            act->destroy(act);
        }

        // Refresh the UI.
        int scroll_index = user_interface_refresh_output_window(state->ui, state->active_session->output_data);
        scrollback_set_scroll(state->active_session->output_data, scroll_index);
        user_interface_refresh_input_line_window(state->ui, state->active_session->input_data);
    }
}

static void resize_cb(struct ev_loop *loop, struct ev_signal *w, int revents)
{
    app_state *state = ((signal_with_state*)w)->state;
    session *active_session = state->active_session;

    // Restart ncurses.
    endwin();
    refresh();
    clear();

    // Resize things.
    user_interface_resize(state->ui, LINES, COLS);

    // Refresh the output window.
    // Note that the scrollback index is adjusted based on the last line
    // written in the window. This helps cap the scrollback with the
    // window size.
    int scroll_index = user_interface_refresh_output_window(state->ui, active_session->output_data);
    scrollback_set_scroll(active_session->output_data, scroll_index);

    // Refresh the input line.
    user_interface_refresh_input_line_window(state->ui, active_session->input_data);
}

static void server_data_cb(EV_P_ struct ev_io *w, int revents)
{
    connection_io *io_state = (connection_io*)w;
    session *active_session = io_state->state->active_session;
    session *sess = io_state->sess;

    // Read in the data from the server.
    int received = mud_connection_receive(sess->connection, io_state->recv_buffer, MAIN_BUFFER_MAX_SIZE);
    if(received > 0) {
        // Add the data to scrollback.
        scrollback_write(sess->output_data, io_state->recv_buffer, received);

        if(sess == active_session) {
            // Refresh the output window.
            int scroll_index = user_interface_refresh_output_window(io_state->state->ui, sess->output_data);
            scrollback_set_scroll(sess->output_data, scroll_index);
        }
    }

    // Check if the connection is still intact.
    bool connected = mud_connection_connected(sess->connection);
    if(!connected) {
        // TODO: Handle this case!
        ev_io_stop(EV_A_ w);
        free(io_state);
        return;
    }
}

static void add_new_session(app_state *state, struct ev_loop *loop, const char *host, const char* port)
{
    // Create the session.
    session *new_session = session_create(
        mud_connection_create(REAL_SOCKET_OPS),
        scrollback_create(line_buffer_create(10000)),
        history_create(line_buffer_create(100)),
        input_line_create(),
        key_binding_table_create());
    key_binding_table_set_binding(new_session->bindings, 9, (action*)next_session_action_create(&state->active_session)); //TAB

    // Add it to the session list.
    if(state->active_session == NULL) {
        new_session->next = new_session;
        new_session->prev = new_session;
    } else {
        new_session->next = state->active_session->next;
        new_session->prev = state->active_session;
        new_session->next->prev = new_session;
        new_session->prev->next = new_session;
    }

    // Make the session the active one.
    state->active_session = new_session;

    // Connect to the server.
    bool connected = mud_connection_connect(new_session->connection, host, port);
    if(!connect) {
        color_string *msg = color_string_create_from_c_str("Could not connect.");
        scrollback_write(new_session->output_data, color_string_get_data(msg),
            color_string_length(msg));
        color_string_destroy(msg);

        // Refresh the output window.
        int scroll_index = user_interface_refresh_output_window(state->ui, new_session->output_data);
        scrollback_set_scroll(new_session->output_data, scroll_index);
        return;
    }

    // Register a connection handler (memory leak known).
    connection_io *connection_watcher = malloc(sizeof(connection_io));
    connection_watcher->state = state;
    connection_watcher->sess = new_session;
    ev_io_init(&connection_watcher->io, server_data_cb,
        new_session->connection->_fd, EV_READ);
    ev_io_start(loop, &connection_watcher->io);
}

void main_with_guile(void *data, int argc, char **argv)
{
    if(argc != 3 && data == NULL) {
        printf("Usage: mud <host> <port>\n");
        exit(1);
    }

    // SET UP SIGNAL HANDLERS.
    // Purposely ignore SIGPIPE. When socket errors occur, they should
    // be checked for and handled at the location they occur.
    signal(SIGPIPE, SIG_IGN);

    // OPEN THE SYSLOG.
    openlog("mud", 0x00, LOG_USER);

    // SET UP GUILE.
    init_guile();

    // SET UP THE SESSION.
    /*session *main_session = session_create(
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
    }*/

    // SET UP THE USER INTERFACE.
    // Start ncurses.
    init_ncurses();

    // Create the UI.
    user_interface *ui = user_interface_create(0, 0, LINES, COLS);

    // Setup application state.
    app_state state;
    state.active_session = NULL;
    state.ui = ui;

    // SETUP MAIN LOOP.
    struct ev_loop *loop = ev_default_loop(0);

    // User input watcher.
    io_with_state user_input_watcher;
    user_input_watcher.state = &state;
    ev_io_init(&user_input_watcher.io, user_input_cb, 0 /* STDIN FD */, EV_READ);
    ev_io_start(loop, &user_input_watcher.io);

    // Resize signal watcher.
    signal_with_state resize_watcher;
    resize_watcher.state = &state;
    ev_signal_init(&resize_watcher.signal, resize_cb, SIGWINCH);
    ev_signal_start(loop, &resize_watcher.signal);

    // Create a session.
    const char *host = argv[1];
    const char *port = argv[2];
    add_new_session(&state, loop, host, port);
    add_new_session(&state, loop, "waterdeep.org", "4200");

    // MUD server data received watcher. (will be added on the fly for multiple sessions).
    /*connection_io connection_watcher;
    connection_watcher.state = &state;
    connection_watcher.sess = main_session;
    ev_io_init(&connection_watcher.io, server_data_cb,
        main_session->connection->_fd, EV_READ);
    ev_io_start(loop, &connection_watcher.io);*/

    // MAIN LOOP.
    ev_loop(loop, 0);

    // CLEAN UP.
    end_ncurses();
    session *curr = state.active_session;
    curr->prev->next = NULL;
    while(curr != NULL) {
        session *next = curr->next;
        session_destroy(curr);
        curr = next;
    }
    user_interface_destroy(ui);
}

int main(int argc, char **argv)
{
    scm_boot_guile(argc, argv, &main_with_guile, NULL);

    // Never reached.
    return 0;
}
