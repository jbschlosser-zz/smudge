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

#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "mud_connection.h"

mud_connection *mud_connection_create(socket_ops sock_ops)
{
    mud_connection *mc = malloc(sizeof(mud_connection));
    memset(mc, 0x00, sizeof(mud_connection));
    mc->_sock_ops = sock_ops;
    mc->_telnet = telnet_create();
    mc->_esc_sequence = esc_sequence_create();

    return mc;
}

void mud_connection_destroy(mud_connection *mc)
{
    if(!mc) return;

    close(mc->_fd);
    telnet_destroy(mc->_telnet);
    esc_sequence_destroy(mc->_esc_sequence);
    free(mc);
}

bool mud_connection_connect(mud_connection *mc, const char *hostname, const char *port_number)
{
    if(!mc) return false;

    struct addrinfo *address_info;
    bool addr_info_retrieved = (mc->_sock_ops.getaddrinfo(hostname, port_number, NULL, &address_info) == 0);
    if(!addr_info_retrieved) {
        return false;
    }
    mc->_fd = mc->_sock_ops.socket(address_info->ai_family, address_info->ai_socktype, address_info->ai_protocol);
    bool fd_valid = (mc->_fd != -1);
    if(!fd_valid) {
        mc->_sock_ops.freeaddrinfo(address_info);
        return false;
    }
    bool connected = (mc->_sock_ops.connect(mc->_fd, address_info->ai_addr, address_info->ai_addrlen) == 0);
    if(!connected) {
        mc->_sock_ops.freeaddrinfo(address_info);
        return false;
    }
    int blocking_mode = 1; // 1 corresponds to non-blocking input.
    bool input_blocking_disabled = (mc->_sock_ops.ioctl(mc->_fd, FIONBIO, &blocking_mode) == 0);
    if(!input_blocking_disabled) {
        close(mc->_fd);
        mc->_sock_ops.freeaddrinfo(address_info);
        return false;
    }
    mc->_sock_ops.freeaddrinfo(address_info);
    mc->_connected = true;
    return true;
}

static void handle_telnet_command(mud_connection *mc, unsigned char *cmd, int cmd_len)
{
    // TODO: Implement this.
    return;
}

// Returns the mud character form of the byte if the byte should
// be displayed. Otherwise (e.g. the byte contributes to a telnet
// command or an escape sequence), 0 is returned.
static color_char process_byte(mud_connection *mc, unsigned char byte)
{
    if(!mc) return 0;

    // Check if the byte contributes to a telnet command. Note that telnet
    // commands can arrive in the middle of escape sequences and must be handled
    // first.
    int telnet_state_updated = telnet_update(mc->_telnet, byte);
    if(telnet_state_updated) {
        if(mc->_telnet->cmd_ready) {
            // Handle the command.
            handle_telnet_command(mc, mc->_telnet->cmd, mc->_telnet->cmd_len);

            // Clear the telnet command to indicate that it has been handled.
            telnet_clear_cmd(mc->_telnet);
        }
        return 0;
    }

    // Check if the byte contributes to an escape sequence.
    int esc_sequence_state_updated = esc_sequence_update(mc->_esc_sequence, byte);
    if(esc_sequence_state_updated) {
        if(mc->_esc_sequence->ready) {
            // Handle the escape sequence.
            color_char new_char_attrs = esc_sequence_get_char_attrs(mc->_esc_sequence);
            if(new_char_attrs < INT_MAX) {
                mc->_current_char_attrs = new_char_attrs;
            }

            // Clear the escape sequence to indicate that it has been handled.
            esc_sequence_clear(mc->_esc_sequence);
        }
        return 0;
    }

    // Ignore carriage returns.
    if(byte == 0xD) {
        return 0;
    }

    return byte;
}

int mud_connection_receive(mud_connection *mc, color_char *receive_buf, int len)
{
    if(!mc) return -1;
    if(!mc->_connected) return -1;
    if(!receive_buf) return -1;
    if(len < 0) return -1;
    if(len == 0) return 0;

    int total_received = 0;
    while(total_received < len) {
        // Receive some bytes.
        int left_to_receive = (len - total_received);
        int bytes_to_receive = (left_to_receive < RECV_BUFFER_MAX_SIZE ? left_to_receive : RECV_BUFFER_MAX_SIZE);
        int received = mc->_sock_ops.recv(mc->_fd, mc->_recv_buf, bytes_to_receive, 0);
        if(received < 0) {
            if(errno == EWOULDBLOCK) {
                // There is no data to read.
                break;
            } else {
                // Something went wrong...
                mc->_connected = false;
                return -1;
            }
        }
        if(received == 0) {
            // A return of value of 0 indicates that the connection is broken.
            mc->_connected = false;
            return -1;
        }

        // Process the received bytes.
        int receive_index;
        for(receive_index = 0; receive_index < received; ++receive_index) {
            color_char result = process_byte(mc, mc->_recv_buf[receive_index]);

            // Add the character to the output buffer.
            if(result) {
                receive_buf[total_received++] = mc->_recv_buf[receive_index] | mc->_current_char_attrs;
            }
        }
    }

    return total_received;
}

size_t mud_connection_send(mud_connection *mc, char *send_buf, int len)
{
    if(!mc) return -1;
    if(!mc->_connected) return -1;

    // TODO: Handle SIGPIPE with signal(SIGPIPE, SIG_IGN) and add logic here
    // to check for write errors.
    return mc->_sock_ops.send(mc->_fd, send_buf, len, 0);
}

size_t mud_connection_send_command(mud_connection *mc, char *send_buf, int len)
{
    char *command_buf = malloc(len + 2);
    memcpy(command_buf, send_buf, len);
    command_buf[len] = 13; // CR.
    command_buf[len + 1] = 10; // LF.
    size_t result = mud_connection_send(mc, command_buf, len + 2);
    free(command_buf);

    return result;
}

bool mud_connection_connected(mud_connection *mc)
{
    if(!mc) return false;

    return mc->_connected;
}
