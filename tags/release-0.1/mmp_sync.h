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
/** \file   mmp_sync.h
 * \brief   various syncronization objects
 * \author  FtM
 * \date    2010-Jul-08
 *
 * The names for syncronization objects are as follows:
 * - semaphore => semaphore.
 * - mutex => binary semaphore, acquirable/releasable only by the same thread.
 * - event => manually (re)settable binary semaphore.
 */
#ifndef H_MMP_SYNC_H
#define H_MMP_SYNC_H

#ifdef _WIN32
#   include <windows.h>
    typedef HANDLE t_mmp_thr_sem;
    typedef HANDLE t_mmp_thr_mtx;
    typedef HANDLE t_mmp_thr_evt;
#   define MMP_THRSEM_INVALID INVALID_HANDLE_VALUE
#   define MMP_THRMTX_INVALID INVALID_HANDLE_VALUE
#   define MMP_THREVT_INVALID INVALID_HANDLE_VALUE
#else
#   ifndef _XOPEN_SOURCE
#      define _XOPEN_SOURCE 500        /* for pthread_mutexattr_settype */
#   endif
#   include <pthread.h>
#   include <semaphore.h>
    /** semaphore type */
    typedef sem_t *t_mmp_thr_sem;
    /** mutex type */
    typedef pthread_mutex_t *t_mmp_thr_mtx;
    /** event type */
    typedef sem_t *t_mmp_thr_evt;
#   define MMP_THRSEM_INVALID NULL
#   define MMP_THRMTX_INVALID NULL
#   define MMP_THREVT_INVALID NULL
#endif

/** \def MMP_THRSEM_INVALID
 * \brief invalid semaphore constant */
/** \def MMP_THRMTX_INVALID
 * \brief invalid mutex constant */
/** \def MMP_THREVT_INVALID
 * \brief invalid event constant */

#include "mmp_h_utils.h"
#include "mmp_trace.h"
#include "mmp_memory.h"

MMP_CEXTERN_BEGIN

/* semaphore*/
/** create semaphore */
MMP_API t_mmp_thr_sem mmp_thr_sem_create(long initCount, long maxCount);

/** destroy semaphore */
MMP_API ret_t mmp_thr_sem_close(t_mmp_thr_sem *sem);

/** lock semaphore */
MMP_API ret_t mmp_thr_sem_lock(t_mmp_thr_sem sem);

/** release semaphore */
MMP_API ret_t mmp_thr_sem_release(t_mmp_thr_sem sem);


/* mutex */
/** create mutex */
MMP_API t_mmp_thr_mtx mmp_thr_mtx_create(void);

/** destroy mutex */
MMP_API ret_t mmp_thr_mtx_close(t_mmp_thr_mtx *mtx);

/** lock mutex */
MMP_API ret_t mmp_thr_mtx_lock(t_mmp_thr_mtx mtx);

/** release mutex */
MMP_API ret_t mmp_thr_mtx_release(t_mmp_thr_mtx mtx);


/* event */
/** create event */
MMP_API t_mmp_thr_evt mmp_thr_evt_create();

/** destroy event */
MMP_API ret_t mmp_thr_evt_close(t_mmp_thr_evt *evt);

/** signal an event */
MMP_API ret_t mmp_thr_evt_signal(t_mmp_thr_evt evt);

/** wait on event */
MMP_API ret_t mmp_thr_evt_wait(t_mmp_thr_evt evt);

MMP_CEXTERN_END

#endif /* H_MMP_SYNC_H */
