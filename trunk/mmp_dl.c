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
#include "mmp_dl.h"

#ifndef _WIN32
#   define  HANDLE  void*
#else
#   define HANDLE   HMODULE
#endif

void *mmp_dl_open_and_get_fnc(char *filename, char *getfnc_name)
{
    HANDLE handle;
    void *ret;
    if (filename==NULL || filename[0]=='\0' || 
            getfnc_name==NULL || getfnc_name[0]=='\0') {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
#ifndef _WIN32
    if ((handle = dlopen(filename, RTLD_NOW | RTLD_GLOBAL))==NULL) {
        mmp_setError_ext(MMP_ERR_DL, dlerror());
        return NULL;
    }
    dlerror();
    if ((ret = dlsym(handle, getfnc_name))==NULL)
        mmp_setError_ext(MMP_ERR_DL, dlerror());
#else
    if ((handle = LoadLibrary(filename))==NULL) {
        mmp_setError(MMP_ERR_DL);
        /*mmp_setError_ext(MMP_ERR_DL, GetLastError());*/
        return NULL;
    }
#pragma warning(push)
#pragma warning(disable:4152) /* disable non-standard extension warning */
    if ((ret = GetProcAddress(handle, getfnc_name))==NULL)
#pragma warning(pop)
        mmp_setError(MMP_ERR_DL);
        /*mmp_setError_ext(MMP_ERR_DL, GetLastError());*/
#endif
    return ret;
}

