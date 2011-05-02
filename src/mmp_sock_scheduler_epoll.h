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
/** \file   mmp_sock_scheduler_epoll.h
 * \brief   socket scheduler functions (epoll implementation)
 * \author  FtM
 * \date    2011-May-02
 */

#ifndef H_MMP_SOCK_SCHEDULER_EPOLL_H
#define H_MMP_SOCK_SCHEDULER_EPOLL_H
#include "mmp_sock_scheduler.h"

#ifdef BUILD_EPOLL_SCHEDULER
MMP_CEXTERN_BEGIN
MMP_API t_mmp_sched_id mmp_scheduler_epoll_create(size_t pool_size);
MMP_API void mmp_scheduler_epoll_destroy(t_mmp_sched_id sched_id);
MMP_API ret_t mmp_scheduler_epoll_add_listen_socket(t_mmp_sched_id sched_id,
                                                    t_socket sock);
MMP_API ret_t mmp_scheduler_epoll_add_client_socket(t_mmp_sched_id sched_id,
                                                    t_socket sock);
MMP_API ret_t mmp_scheduler_epoll_del_socket(t_mmp_sched_id sched_id,
                                             		t_socket sock);
MMP_API t_mmp_sched_ret_e mmp_scheduler_epoll_loop(t_mmp_sched_id sched_id,
                                                   t_mmp_schedfnc_fp cback_fp,
			                                       int millisecs,
                                                   t_mmp_schedto_fp cback_to);
MMP_CEXTERN_END
#endif /* BUILD_EPOLL_SCHEDULER */

#endif /* H_MMP_SOCK_SCHEDULER_EPOLL_H */
