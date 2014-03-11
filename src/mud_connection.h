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

#ifndef MSOCKET_H
#define MSOCKET_H

#include <stdbool.h>
#include "esc_sequence.h"
#include "color_string.h"
#include "socket_ops.h"
#include "telnet.h"

#define RECV_BUFFER_MAX_SIZE 4096

// Connection a MUD server.
typedef struct {
    socket_ops _sock_ops;
    int _fd;
    bool _connected;
    char _recv_buf[RECV_BUFFER_MAX_SIZE];
    telnet *_telnet; // Telnet state machine.
    esc_sequence *_esc_sequence; // ESC sequence state machine.
    color_char _current_char_attrs; // Contains the most recent set of character attributes
                                    // (e.g. bold, blink, color, etc.) to apply to text.
} mud_connection;

// Construction/destruction.
mud_connection *mud_connection_create(socket_ops sock_ops);
void mud_connection_destroy(mud_connection *mc);

// Member functions.
bool mud_connection_connect(mud_connection *mc, const char *hostname, const char *port_number);
int mud_connection_receive(mud_connection *mc, color_char *receive_buf, int len);
size_t mud_connection_send(mud_connection *mc, char *send_buf, int len);
size_t mud_connection_send_command(mud_connection *mc, char *send_buf, int len);
bool mud_connection_connected(mud_connection *mc);

#endif
