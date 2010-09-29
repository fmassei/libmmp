/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "mmp_socket.h"

/** \todo missing unittest */
ret_t mmp_socket_initSystem(void)
{
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData)!=0) {
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
#endif
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_socket_finiSystem(void)
{
#ifdef _WIN32
    WSACleanup();
#endif
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_socket_server_start(int port, int qsize, t_socket *sock)
{
#ifndef _WIN32
    struct sockaddr_in sa;
    int raddr = 1;
    if ((*sock = socket(AF_INET, SOCK_STREAM, 0))<0) {
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &raddr, sizeof(raddr));
    memset((char*)&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_addr.s_addr = INADDR_ANY;
    sa.sin_port = htons(port);
    if (bind(*sock, (struct sockaddr*)&sa, sizeof(sa))<0) {
        mmp_socket_close(sock, 0);
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    if (listen(*sock, qsize)<0) {
        mmp_socket_close(sock, 0);
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
#else
    struct addrinfo * __restrict result = NULL, hints;
    char buf[10];
    sprintf(buf, "%d", port);
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;
    if (getaddrinfo(NULL, buf, &hints, &result)!=0) {
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    *sock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (*sock == INVALID_SOCKET) {
        freeaddrinfo(result);
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    if (bind(*sock, result->ai_addr, (int)result->ai_addrlen)==SOCKET_ERROR) {
        freeaddrinfo(result);
        mmp_socket_close(sock, 0);
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
    freeaddrinfo(result);
    if (listen(*sock, qsize)==SOCKET_ERROR) {
        mmp_socket_close(sock, 0);
        mmp_setError(MMP_ERR_SOCKET);
        return MMP_ERR_SOCKET;
    }
#endif
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_socket_close(t_socket *sock, int shut)
{
#ifndef _WIN32
    if (shut)
        shutdown(*sock, SHUT_RDWR);
    close(*sock);
#else
    if (shut)
        shutdown(*sock, SD_BOTH);
    closesocket(*sock);
#endif
    *sock = SOCKET_INVALID;
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_socket_server_accept(const t_socket * __restrict listen_sock,
                                    t_socket * __restrict out, char **ip)
{
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    *out = accept(*listen_sock, (struct sockaddr *)&addr, &addrlen);
    if ((*ip = xstrdup(inet_ntoa(addr.sin_addr)))==NULL) {
        /** \todo do nothing? */
    }
    return MMP_ERR_OK;
}

/** \todo missing unittest */
int mmp_socket_server_select(int nfds, fd_set *rd, fd_set *wd, fd_set *ex,
                                            struct timeval * __restrict to)
{
    int ret;
#ifndef _WIN32
    if ((ret = select(nfds, rd, wd, ex, to))<0) {
#else
    if ((ret = select(nfds, rd, wd, ex, to))==SOCKET_ERROR) {
#endif
        mmp_setError(MMP_ERR_SOCKET);
        return -1;
    }
    return ret;
}

/** \todo missing unittest */
int mmp_socket_read(const t_socket * __restrict sock,
                                    void * __restrict buf, size_t len)
{
    return recv(*sock, buf, len, 0);
}

/** \todo missing unittest */
int mmp_socket_write(const t_socket * __restrict sock,
                                    const void * __restrict buf, size_t len)
{
    return send(*sock, buf, len, 0);
}

/** \todo missing unittest */
int mmp_socket_set_nonblocking(const t_socket *sock)
{
#ifdef _WIN32
    unsigned long mode = 1;
    return ioctlsocket(*sock, FIONBIO, &mode);
#else
    int result;
    result = fcntl(*sock, F_GETFL, NULL);
    result |= O_NONBLOCK;
    return fcntl(*sock, F_SETFL, result);
#endif
}

/** \todo missing unittest */
int mmp_socket_is_block_last_error(void)
{
#ifdef _WIN32
    if (WSAGetLastError()==WSAEWOULDBLOCK)
        return 1;
#else
    if (errno==EAGAIN || errno==EWOULDBLOCK)
        return 1;
#endif
    return 0;
}
