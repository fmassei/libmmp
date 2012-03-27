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
#include "mmp_sock_scheduler_fake.h"

t_mmp_sched_id mmp_scheduler_fake_create(size_t pool_size)
{
    mmp_trace_reset();
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return -1;
}

void mmp_scheduler_fake_destroy(t_mmp_sched_id sched_id)
{
}

ret_t mmp_scheduler_fake_add_listen_socket(t_mmp_sched_id sched_id,
                                           t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

ret_t mmp_scheduler_fake_add_client_socket(t_mmp_sched_id sched_id,
                                           t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

ret_t mmp_scheduler_fake_del_socket(t_mmp_sched_id sched_id, t_socket sock)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_ERR_GENERIC;
}

t_mmp_sched_ret_e mmp_scheduler_fake_loop(t_mmp_sched_id sched_id,
                                          t_mmp_schedfnc_fp cback_fp,
	                                      int millisecs,
                                          t_mmp_schedto_fp cback_to)
{
    mmp_setError_ext(MMP_ERR_GENERIC, "Could not work with fake scheduler!");
    return MMP_SCHEDRET_ERR;
}

