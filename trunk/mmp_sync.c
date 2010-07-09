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
#include "mmp_sync.h"

#ifdef _WIN32
#   pragma warning(push)
#   pragma warning(disable:4127) /* disable warnings relatives to do-while(0)
                                  * constructs. */
#endif
#define SETERR_AND_RETURN(_ERR) do { \
        mmp_setError((_ERR)); \
        return (_ERR); \
    } while(0)

/* ***************************************************************************
 * semaphores
 * **************************************************************************/
/** \todo missing unittest */
t_mmp_thr_sem mmp_thr_sem_create(long initCount, long maxCount)
{
    t_mmp_thr_sem ret;
#ifdef _WIN32
    ret = CreateSemaphore(NULL, initCount, maxCount, NULL);
    if (ret==INVALID_HANDLE_VALUE) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_THRSEM_INVALID;
    }
#else
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if (sem_init(ret, 0, initCount)!=0) {
        mmp_setError(MMP_ERR_SYNC);
        xfree(ret);
        return MMP_THRSEM_INVALID;
    }
#endif
    return ret;
}

/** \todo missing unittest */
ret_t mmp_thr_sem_close(t_mmp_thr_sem *sem)
{
    if (sem==NULL || *sem==MMP_THRSEM_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (CloseHandle(*sem)!=TRUE)
        return MMP_ERR_SYNC; /* we don't trace the error, as it could be a
                              * desired behaviour */
#else
    if (sem_destroy(*sem)!=0)
        return MMP_ERR_SYNC; /* like above */
    xfree(*sem);
#endif
    *sem = MMP_THRSEM_INVALID;
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_sem_lock(t_mmp_thr_sem sem)
{
    if (sem==MMP_THRSEM_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (WaitForSingleObject(sem, INFINITE)!=WAIT_OBJECT_0)
#else
    if (sem_wait(sem)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_sem_release(t_mmp_thr_sem sem)
{
    if (sem==MMP_THRSEM_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (ReleaseSemaphore(sem, 1, NULL)!=TRUE)
#else
    if (sem_post(sem)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

/* ***************************************************************************
 * mutexes
 * **************************************************************************/
/** \todo missing unittest */
t_mmp_thr_mtx mmp_thr_mtx_create(void)
{
    t_mmp_thr_mtx ret;
#ifdef _WIN32
    ret = CreateMutex(NULL, FALSE, NULL);
    if (ret==INVALID_HANDLE_VALUE) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_THRMTX_INVALID;
    }
#else
    pthread_mutexattr_t mutexattr;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_THRMTX_INVALID;
    }
    pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
    if (pthread_mutex_init(ret, &mutexattr)!=0) {
        mmp_setError(MMP_ERR_SYNC);
        xfree(ret);
        return MMP_THRMTX_INVALID;
    }
#endif
    return ret;
}

/** \todo missing unittest */
ret_t mmp_thr_mtx_close(t_mmp_thr_mtx *mtx)
{
    if (mtx==NULL || *mtx==MMP_THRMTX_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (CloseHandle(*mtx)!=TRUE)
        return MMP_ERR_SYNC; /* we don't trace the error, as it could be a
                             * desired behaviour */
#else
    if (pthread_mutex_destroy(*mtx)!=0)
        return MMP_ERR_SYNC; /* like above */
    xfree(*mtx);
#endif
    *mtx = MMP_THRMTX_INVALID;
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_mtx_lock(t_mmp_thr_mtx mtx)
{
    if (mtx==MMP_THRMTX_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (WaitForSingleObject(mtx, INFINITE)!=WAIT_OBJECT_0)
#else
    if (pthread_mutex_lock(mtx)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_mtx_release(t_mmp_thr_mtx mtx)
{
    if (mtx==MMP_THRMTX_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (ReleaseMutex(mtx)!=TRUE)
#else
    if (pthread_mutex_unlock(mtx)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

/* ***************************************************************************
 * events
 * **************************************************************************/
/** \todo missing unittest */
t_mmp_thr_evt mmp_thr_evt_create()
{
    t_mmp_thr_evt ret;
#ifdef _WIN32
    ret = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (ret==INVALID_HANDLE_VALUE) {
        mmp_setError(MMP_ERR_SYNC);
        return MMP_THREVT_INVALID;
    }
#else
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_THREVT_INVALID;
    }
    if (sem_init(ret, 0, 0)!=0) {
        mmp_setError(MMP_ERR_SYNC);
        xfree(ret);
        return MMP_THREVT_INVALID;
    }
#endif
    return ret;
}

/** \todo missing unittest */
ret_t mmp_thr_evt_close(t_mmp_thr_evt *evt)
{
    if (evt==NULL || *evt==MMP_THREVT_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (CloseHandle(*evt)!=TRUE)
        return MMP_ERR_SYNC;
#else
    if (sem_destroy(*evt)!=0)
        return MMP_ERR_SYNC;
    xfree(*evt);
#endif
    *evt = MMP_THREVT_INVALID;
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_evt_signal(t_mmp_thr_evt evt)
{
    if (evt==MMP_THREVT_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (SetEvent(evt)!=TRUE)
#else
    if (sem_post(evt)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_thr_evt_wait(t_mmp_thr_evt evt)
{
    if (evt==MMP_THREVT_INVALID)
        SETERR_AND_RETURN(MMP_ERR_PARAMS);
#ifdef _WIN32
    if (WaitForSingleObject(evt, INFINITE)!=WAIT_OBJECT_0)
#else
    if (sem_wait(evt)!=0)
#endif
        SETERR_AND_RETURN(MMP_ERR_SYNC);
    return MMP_ERR_OK;
}

#ifdef _WIN32
#   pragma warning(pop)
#endif
