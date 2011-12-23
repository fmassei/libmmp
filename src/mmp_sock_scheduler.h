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
/** \file   mmp_sock_scheduler.h
 * \brief   socket scheduler functions
 * \author  FtM
 * \date    2011-May-02
 */
#ifndef H_MMP_SOCK_SCHEDULER_H
#define H_MMP_SOCK_SCHEDULER_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmp_error.h"
#include "mmp_socket.h"
#include "mmp_compat.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"

typedef int t_mmp_sched_id;

typedef enum mmp_sched_ret_e {
    MMP_SCHEDRET_OK,
    MMP_SCHEDRET_ERR,
    MMP_SCHEDRET_CBACKERR,
} t_mmp_sched_ret_e;
typedef enum mmp_schedfnc_ret_e {
    MMP_SCHEDFNCRET_OK,
    MMP_SCHEDFNCRET_ERR
} t_mmp_schedfnc_ret_e;

/* callback function */
typedef t_mmp_schedfnc_ret_e(*t_mmp_schedfnc_fp)(t_socket);
/* timeout function */
typedef void(*t_mmp_schedto_fp)(void);

#ifdef HAVE_SYS_EPOLL_H
#   define BUILD_EPOLL_SCHEDULER
#   include "mmp_sock_scheduler_epoll.h"
#   define mmp_scheduler_create            mmp_scheduler_epoll_create
#   define mmp_scheduler_destroy           mmp_scheduler_epoll_destroy
#   define mmp_scheduler_add_listen_socket mmp_scheduler_epoll_add_listen_socket
#   define mmp_scheduler_add_client_socket mmp_scheduler_epoll_add_client_socket
#   define mmp_scheduler_del_socket        mmp_scheduler_epoll_del_socket
#   define mmp_scheduler_loop              mmp_scheduler_epoll_loop
#else
#   define BUILD_FAKE_SCHEDULER
#   include "mmp_sock_scheduler_fake.h"
#   define mmp_scheduler_create            mmp_scheduler_fake_create
#   define mmp_scheduler_destroy           mmp_scheduler_fake_destroy
#   define mmp_scheduler_add_listen_socket mmp_scheduler_fake_add_listen_socket
#   define mmp_scheduler_add_client_socket mmp_scheduler_fake_add_client_socket
#   define mmp_scheduler_del_socket        mmp_scheduler_fake_del_socket
#   define mmp_scheduler_loop              mmp_scheduler_fake_loop
#endif

#endif /* H_MMP_SOCK_SCHEDULER_H */
