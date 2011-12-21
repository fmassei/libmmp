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

