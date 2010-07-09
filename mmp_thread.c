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
#include "mmp_thread.h"

/** \test mmp_thread_unittest */
ret_t mmp_thread_create(t_mmp_thread_start fnc, void *arg, t_mmp_thread *out)
{
#ifndef _WIN32
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setstacksize(&threadAttr, 4*1024);
    pthread_create(out, &threadAttr, fnc, arg);
    pthread_attr_destroy(&threadAttr);
#else
    DWORD id;
    *out = CreateThread(NULL, 4*1024, (LPTHREAD_START_ROUTINE)fnc, arg, 0, &id);
#endif
    return MMP_ERR_OK;
}

/** \test mmp_thread_unittest */
ret_t mmp_thread_join(t_mmp_thread *id)
{
#ifndef _WIN32
    pthread_join(*id, NULL);
#else
    WaitForSingleObject(*id, INFINITE);
#endif
    return MMP_ERR_OK;
}

/** \test mmp_thread_unittest */
void mmp_thread_exit(int code)
{
#ifndef _WIN32
    pthread_exit(NULL);
#else
    ExitThread((DWORD)code);
#endif
}

#ifdef UNIT_TESTING
int glob;
static void* thtest(void* ptr)
{
    ++glob;
    mmp_thread_exit(0);
    ptr = ptr;
    return NULL;
}
static t_mmp_tap_result_e test_threads(void)
{
    int i;
    t_mmp_thread id[10];
    glob = 0;
    for (i=0; i<10; ++i) {
        if (mmp_thread_create(thtest, NULL, &(id[i]))!=MMP_ERR_OK)
            return MMP_TAP_FAILED;
    }
    for (i=0; i<10; ++i)
        if (mmp_thread_join(&(id[i]))!=MMP_ERR_OK)
            return MMP_TAP_FAILED;
    if (glob!=10)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
ret_t mmp_thread_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp_thread funcs", NULL,
                                        test_threads()))!=MMP_ERR_OK)
        )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
