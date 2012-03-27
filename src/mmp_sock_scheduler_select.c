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
#include "mmp_sock_scheduler_select.h"
#ifdef HAVE_SELECT

#include <sys/select.h>
#include "mmp_gen_vect.h"

static t_mmp_ivect_s *s_fds;
static int s_nfds, s_highestfd;

t_mmp_sched_id mmp_scheduler_select_create(size_t pool_size)
{
    t_mmp_sched_id epollfd = 1;
    int i;
    if ((s_fds = mmp_ivect_create(FD_SETSIZE))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    for (i=0; i<s_fds->n_data; ++i)
        s_fds->data[i] = -1;
    s_nfds = s_highestfd = 0;
    return epollfd;
}

void mmp_scheduler_select_destroy(t_mmp_sched_id sched_id)
{
    mmp_ivect_destroy(&s_fds);
}

ret_t mmp_scheduler_select_add_listen_socket(t_mmp_sched_id sched_id,
                                            t_socket sock)
{
    return mmp_scheduler_select_add_client_socket(sched_id, sock);
}

ret_t mmp_scheduler_select_add_client_socket(t_mmp_sched_id sched_id,
                                            t_socket sock)
{
    mmp_ivect_push(s_fds, sock);
    if (sock+1>s_nfds) s_nfds = sock+1;
    return MMP_ERR_OK;
}

ret_t mmp_scheduler_select_del_socket(t_mmp_sched_id sched_id, t_socket sock)
{
    int i;
    for (i=0; i<s_fds->n_data; ++i) {
        if (s_fds->data[i]==sock) {
            mmp_ivect_del(s_fds, i);
            break;
        }
    }
    return MMP_ERR_OK;
}

t_mmp_sched_ret_e mmp_scheduler_select_loop(t_mmp_sched_id sched_id,
                                           t_mmp_schedfnc_fp cback_fp,
	                                       int millisecs,
                                           t_mmp_schedto_fp cback_to)
{
    int nret, cback_err = 0, i;
    struct timeval tv;
    fd_set rfds;
    tv.tv_sec = 0;
    tv.tv_usec = millisecs*1000;
    FD_ZERO(&rfds);
    for (i=0; i<s_fds->n_data; ++i)
        FD_SET(s_fds->data[i], &rfds);
repoll:
#ifndef WIN32
    if ((nret = select(s_highestfd, &rfds, NULL, NULL, &tv))==-1) {
#else
    if ((nret = select(s_highestfd, &rfds, NULL, NULL, &tv))==SOCKET_ERROR) {
#endif
#ifndef NDEBUG
        if (errno==EINTR) goto repoll; /* FIXME: remove this */
#endif
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_SCHEDRET_ERR;
    }
    if (nret==0) {
        if (cback_to!=NULL)
            cback_to();
        return MMP_SCHEDRET_OK;
    } else {
        for (i=0; i<s_fds->n_data; ++i) {
            if (FD_ISSET(s_fds->data[i], &rfds)) {
                if (cback_fp(s_fds->data[i])!=MMP_SCHEDFNCRET_OK) {
                    mmp_setError(MMP_ERR_GENERIC);
                    cback_err = 1;
                }
            }
        }
    }
    return (cback_err) ? MMP_SCHEDRET_CBACKERR : MMP_SCHEDRET_OK;
}

#endif /* HAVE_SELECT */

