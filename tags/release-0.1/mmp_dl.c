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
#include "mmp_dl.h"

#ifndef _WIN32
#   define  HANDLE  void*
#else
#   define HANDLE   HMODULE
#endif

/** \todo missing unittest */
t_mmp_fp mmp_dl_open_and_get_fnc(const char * __restrict filename,
                              const char * __restrict getfnc_name)
{
    HANDLE handle;
    t_mmp_fp ret;
    MMP_CHECK_OR_RETURN((filename!=NULL && filename[0]!='\0' && 
                        getfnc_name!=NULL && getfnc_name[0]!='\0'), NULL);
#ifndef _WIN32
    if ((handle = dlopen(filename, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        mmp_setError_ext(MMP_ERR_DL, dlerror());
        return NULL;
    }
    dlerror();
    if ((ret = (t_mmp_fp)dlsym(handle, getfnc_name))==NULL) {
        mmp_setError_ext(MMP_ERR_DL, dlerror());
        dlclose(handle);
    }
#else
    if ((handle = LoadLibraryA(filename))==NULL) {
        char buf[0xff];
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf, sizeof(buf), NULL);
        mmp_setError_ext(MMP_ERR_DL, buf);
        return NULL;
    }
#pragma warning(push)
#pragma warning(disable:4152) /* disable non-standard extension warning */
    if ((ret = (t_mmp_fp)GetProcAddress(handle, getfnc_name))==NULL) {
#pragma warning(pop)
        char buf[0xff];
        FormatMessageA(
            FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            buf, sizeof(buf), NULL);
        mmp_setError_ext(MMP_ERR_DL, buf);
        FreeLibrary(handle);
    }
#endif
    return ret;
}

