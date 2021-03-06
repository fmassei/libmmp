/*
    Copyright 2010 Francesco Massei

    This file is part of the libmmp library.

        libmmp is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libmmp is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with libmmp.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file   mmp_socket.h
 * \brief   socket functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_SOCKET_H
#define H_MMP_SOCKET_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <sys/socket.h>
#   include <sys/time.h>
#   include <netinet/in.h>
#   include <arpa/inet.h>
#   include <netdb.h>
#   include <fcntl.h>
#   include <unistd.h>
    typedef int t_socket;
#   define  SOCKET_INVALID  (-1)
#   define SOCKET_ERROR -1
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <winsock2.h>
#   include <windows.h>
#   include <ws2tcpip.h>
    typedef SOCKET t_socket;
#   define SOCKET_INVALID   INVALID_SOCKET
    /* socket error is defined */
#endif
/** \def SOCKET_INVALID
 * \brief invalid socket constant */
/** \def SOCKET_ERROR
 * \brief error on socket operation */
/** \typedef t_socket
 * \brief mmp socket type */

#include "mmp_compat.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

MMP_CEXTERN_BEGIN

/** \brief initialize socket subsystem */
MMP_API ret_t mmp_socket_initSystem(void);

/** \brief finalize socket subsystem */
MMP_API ret_t mmp_socket_finiSystem(void);

/** \brief connect to a given hostname:port, fill addrbuf if not NULL */
ret_t mmp_socket_connect(const char *hostname, int port, t_socket *sock,
                            char *addrbuf, size_t addrbuf_len);

/** \brief starts a socket server */
MMP_API ret_t mmp_socket_server_start(int port, int qsize, t_socket *sock);

/** \brief starts a socket server on the passed interface */
MMP_API ret_t mmp_socket_server_start_bind(int port, int qsize, char *bind_addr,
                                    t_socket *sock);

/** \brief close a socket */
MMP_API ret_t mmp_socket_close(t_socket *sock, int shut);

/** \brief accept a socket */
MMP_API ret_t mmp_socket_server_accept(const t_socket * __restrict listen_sock,
                                t_socket * __restrict out, char **ip);
/** \brief multiplex sockets */
MMP_API int mmp_socket_server_select(int nfds, fd_set *rd, fd_set *wd,
                                fd_set *ex, struct timeval * __restrict to);

/** \brief read from a socket */
MMP_API int mmp_socket_read(const t_socket * __restrict sock,
                                void * __restrict buf, size_t len);

/** \brief write to a socket */
MMP_API int mmp_socket_write(const t_socket * __restrict sock,
                                const void * __restrict buf, size_t len);

/** \brief set socket as non-blocking */
MMP_API int mmp_socket_set_nonblocking(const t_socket *sock);

/** \brief was the last error an EWOULDBLOCK error? */
MMP_API int mmp_socket_is_block_last_error(void);

MMP_CEXTERN_END

#endif /* H_MMP_SOCKET_H */
