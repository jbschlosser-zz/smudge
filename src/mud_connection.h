#ifndef MSOCKET_H
#define MSOCKET_H

#include <stdbool.h>
#include "esc_sequence.h"
#include "mud_string.h"
#include "socket_ops.h"
#include "telnet.h"

#define RECV_BUFFER_MAX_SIZE 4096

typedef struct {
    socket_ops _sock_ops;
    int _fd;
    bool _connected;
    char _recv_buf[RECV_BUFFER_MAX_SIZE];
    telnet *_telnet;
    esc_sequence *_esc_sequence;
    mud_char_t _current_char_attrs; // Contains the most recent set of character attributes (e.g. bold, blink, color, etc.) to apply to text.
} mud_connection;

// Construction/destruction.
mud_connection *mud_connection_create(socket_ops sock_ops);
void mud_connection_destroy(mud_connection *mc);

// Member functions.
bool mud_connection_connect(mud_connection *mc, const char *hostname, const char *port_number);
int mud_connection_receive(mud_connection *mc, mud_char_t *receive_buf, int len);
size_t mud_connection_send(mud_connection *mc, char *send_buf, int len);
size_t mud_connection_send_command(mud_connection *mc, char *send_buf, int len);
bool mud_connection_connected(mud_connection *mc);

#endif
