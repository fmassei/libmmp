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
#include "mmp_sock_scheduler_epoll.h"
#ifdef BUILD_EPOLL_SCHEDULER

#include <sys/epoll.h>

static struct epoll_event *s_events;
static size_t s_pool_size;

t_mmp_sched_id mmp_scheduler_epoll_create(size_t pool_size)
{
    t_mmp_sched_id epollfd;
    s_pool_size = pool_size;
    if ((s_events = xcalloc(pool_size, sizeof(*s_events)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return -1;
    }
    if ((epollfd = epoll_create(pool_size))==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return -1;
    }
    return epollfd;
}

void mmp_scheduler_epoll_destroy(t_mmp_sched_id sched_id)
{
}

ret_t mmp_scheduler_epoll_add_listen_socket(t_mmp_sched_id sched_id,
                                            t_socket sock)
{
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = sock;
    if (epoll_ctl(sched_id, EPOLL_CTL_ADD, sock, &ev)==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t mmp_scheduler_epoll_add_client_socket(t_mmp_sched_id sched_id,
                                            t_socket sock)
{
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLOUT | EPOLLET;
    ev.data.fd = sock;
    if (epoll_ctl(sched_id, EPOLL_CTL_ADD, sock, &ev)==-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t mmp_scheduler_epoll_del_socket(t_mmp_sched_id sched_id, t_socket sock)
{
    struct epoll_event ev;
    if (epoll_ctl(sched_id, EPOLL_CTL_DEL, sock, &ev)!=-1) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

t_mmp_sched_ret_e mmp_scheduler_epoll_loop(t_mmp_sched_id sched_id,
                                           t_mmp_schedfnc_fp cback_fp,
	                                       int millisecs,
                                           t_mmp_schedto_fp cback_to)
{
    int nfds, n, cback_err = 0;
repoll:
    if ((nfds = epoll_wait(sched_id, s_events, s_pool_size, millisecs))==-1) {
#ifndef NDEBUG
        if (errno==EINTR) goto repoll; /* FIXME: remove this */
#endif
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_SCHEDRET_ERR;
    }
    if (nfds==0) {
        if (cback_to!=NULL)
            cback_to();
        return MMP_SCHEDRET_OK;
    } else {
        for (n=0; n<nfds; ++n) {
            if (cback_fp(s_events[n].data.fd)!=MMP_SCHEDFNCRET_OK) {
                mmp_setError(MMP_ERR_GENERIC);
                cback_err = 1;
            }
        }
    }
    return (cback_err) ? MMP_SCHEDRET_CBACKERR : MMP_SCHEDRET_OK;
}

#endif /* BUILD_EPOLL_SCHEDULER */

