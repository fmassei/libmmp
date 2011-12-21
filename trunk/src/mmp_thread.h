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
/** \file   mmp_thread.h
 * \brief   threads functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_THREAD_H
#define H_MMP_THREAD_H

#include "mmp_h_utils.h"
#ifndef _WIN32
#   include <pthread.h>
    typedef pthread_t t_mmp_thread;
#else
#   ifndef WIN32_LEAN_AND_MEAN
#       define WIN32_LEAN_AND_MEAN
#   endif
#   include <windows.h>
    typedef HANDLE t_mmp_thread;
#endif
/** \typedef t_mmp_thread
 * \brief mmp thread type */

#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief thread starting function type */
typedef void*(*t_mmp_thread_start)(void *);

MMP_CEXTERN_BEGIN

/** \brief create a thread */
MMP_API ret_t mmp_thread_create(t_mmp_thread_start fnc, void *arg,
                                t_mmp_thread *out);

/** \brief join the thread */
MMP_API ret_t mmp_thread_join(t_mmp_thread *id);

/** \brief exit the thread */
MMP_API void mmp_thread_exit(int code);

MMP_CEXTERN_END

#endif /* H_MMP_THREAD_H */
