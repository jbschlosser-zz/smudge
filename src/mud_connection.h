#ifndef MSOCKET_H
#define MSOCKET_H

#include <stdbool.h>
#include "mud_char.h"
#include "socket_ops.h"

#define RECV_BUFFER_MAX_SIZE 4096
#define ESCAPE_SEQUENCE_MAX_SIZE 15
#define ESCAPE_SEQUENCE_BEGIN 0x1B // ESC character.
#define ESCAPE_SEQUENCE_END 0x6D // 'm' character.
#define IAC 0xFF // Interpret As Command. Used for telnet option negotation.
#define TELNET_WILL 251
#define TELNET_WONT 252
#define TELNET_DO 253
#define TELNET_DONT 254

typedef struct
{
    socket_ops _sock_ops;
    int _fd;
    bool _connected;
    char _recv_buf[RECV_BUFFER_MAX_SIZE];
    int _iac_on;
    int _telnet_will_wont_do_dont; // 0 unless in the middle of a negotiation.
    char _escape_sequence[ESCAPE_SEQUENCE_MAX_SIZE];
    int _escape_on;
    int _escape_index;
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

// Other functions.
mud_char_t get_char_attrs(char *escape_seq);

#endif
