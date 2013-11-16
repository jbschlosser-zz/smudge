#include "socket_ops.h"

const socket_ops REAL_SOCKET_OPS = {
    .freeaddrinfo = freeaddrinfo,
    .getaddrinfo = getaddrinfo,
    .socket = socket,
    .connect = connect,
    .select = select,
    .send = send,
    .recv = recv,
    .ioctl = ioctl
};
