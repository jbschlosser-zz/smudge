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

#ifndef SOCKET_OPS_H
#define SOCKET_OPS_H

#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>

// Socket operations. This provides a seam for unit testing
// units that use sockets.
typedef struct
{
    void (*freeaddrinfo) (struct addrinfo *res);
    int (*getaddrinfo) (const char *__restrict __name, const char *__restrict __service, const struct addrinfo *__restrict __req, struct addrinfo **__restrict __pai);
    int (*socket) (int __domain, int __type, int __protocol);
    int (*connect) (int __fd, __CONST_SOCKADDR_ARG __addr, socklen_t __len);
    int (*select) (int __nfds, fd_set *__restrict __readfds, fd_set *__restrict __writefds, fd_set *__restrict __exceptfds, struct timeval *__restrict __timeout);
    ssize_t (*send) (int __fd, const void *__buf, size_t __n, int __flags);
    ssize_t (*recv) (int __fd, void *__buf, size_t __n, int __flags);
    int (*ioctl) (int __fd, unsigned long int __request, ...);
} socket_ops;

extern const socket_ops REAL_SOCKET_OPS;

#endif
