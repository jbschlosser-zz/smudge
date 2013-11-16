#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include "mud_char.h"
#include "mud_connection.h"

mud_connection *mud_connection_create(socket_ops sock_ops)
{
    mud_connection *mc = malloc(sizeof(mud_connection));
    memset(mc, 0x00, sizeof(mud_connection));
    mc->_sock_ops = sock_ops;

    return mc;
}

void mud_connection_destroy(mud_connection *mc)
{
    if(!mc) return;

    close(mc->_fd);
    free(mc);
}

bool mud_connection_connect(mud_connection *mc, const char *hostname, const char *port_number)
{
    if(!mc) return false;

    struct addrinfo *address_info;
    bool addr_info_retrieved = (mc->_sock_ops.getaddrinfo(hostname, port_number, NULL, &address_info) == 0);
    if(!addr_info_retrieved)
        return false;
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

int mud_connection_receive(mud_connection *mc, mud_char_t *receive_buf, int len)
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
            // Check for an IAC.
            unsigned char received_char = mc->_recv_buf[receive_index];
            if(received_char == IAC) {
                if(mc->_iac_on) {
                    // A double IAC corresponds to the data byte 0xFF.
                    mc->_iac_on = 0;
                } else {
                    mc->_iac_on = 1;
                    continue;
                }
            }

            // TODO: Finish option negotiation.
            // Handle Telnet option negotiation.
            if(mc->_iac_on) {
                switch(received_char) {
                    case TELNET_WILL:
                    case TELNET_WONT:
                    case TELNET_DO:
                    case TELNET_DONT:
                        mc->_telnet_will_wont_do_dont = 1;
                        continue;
                    default:
                        // Ignore everything for now.
                        mc->_iac_on = 0;
                        continue;
                }
            }


            // Check for the beginning of an escape sequence.
            if(received_char == ESCAPE_SEQUENCE_BEGIN) {
                mc->_escape_on = 1;
                continue;
            }

            // Check if an escape sequence is in progress.
            if(mc->_escape_on) {
                if(mc->_escape_index < ESCAPE_SEQUENCE_MAX_SIZE - 1) {
                    // Add this character to the escape sequence.
                    mc->_escape_sequence[mc->_escape_index++] = received_char;

                    // Check if the escape sequence has ended.
                    if(received_char == ESCAPE_SEQUENCE_END) {
                        // Get the character attributes that the escape sequence is meant to set.
                        // The attributes will be applied to all characters until the next escape
                        // sequence changes them.
                        mc->_escape_sequence[mc->_escape_index] = '\0';
                        mud_char_t char_attrs = get_char_attrs(mc->_escape_sequence);
                        if(char_attrs < INT_MAX)
                            mc->_current_char_attrs = char_attrs;

                        // Indicate that the escape sequence has finished.
                        mc->_escape_on = 0;
                        mc->_escape_index = 0;
                    }
                } else {
                    // Give up on this escape sequence.
                    mc->_escape_on = 0;
                    mc->_escape_index = 0;
                }

                continue;
            }

            // Ignore carriage returns.
            if(received_char == 0xD)
                continue;

            // Add the character to the output buffer.
            receive_buf[total_received++] = received_char | mc->_current_char_attrs;
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
