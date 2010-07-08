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

#include "mmp_dir.h"

static __inline void freeres(void **res)
{
    xfree(*res);
    *res=NULL;
}

/** \todo missing unittest */
__inline void mmp_dir_free_list_results(t_mmp_dirlist_s **list)
{
    if (list==NULL || *list==NULL)
        return;
    mmp_list_delete_withdata(list, freeres);
}

/** \todo missing unittest */
t_mmp_dirlist_s *mmp_dir_listfiles(const char *dirname)
{
    t_mmp_dirlist_s *ret = NULL;
#ifndef _WIN32
    DIR *dirp;
    struct dirent *de;
#else
    HANDLE hFind;
    WIN32_FIND_DATA FindFileData;
    char *dirpatt;
#endif
    if (dirname==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = mmp_list_create())==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
#ifndef _WIN32
    if ((dirp = opendir(dirname))==NULL) {
        mmp_setError_ext(MMP_ERR_FILE, dirname);
        goto badexit;
    }
    while ((de = readdir(dirp))!=NULL) {
        if (mmp_list_add_data(ret, xstrdup(de->d_name))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            goto badexit;
        }
    }
    if (closedir(dirp)!=0) {
        mmp_setError_ext(MMP_ERR_FILE, dirname);
        goto badexit;
    }
#else
    if ((dirpatt = xmalloc(strlen(dirname)+3))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto badexit;
    }
    strcpy(dirpatt, dirname);
    strcat(dirpatt, "\\*");
    if ((hFind = FindFirstFile(dirpatt, &FindFileData))==INVALID_HANDLE_VALUE) {
        mmp_setError_ext(MMP_ERR_FILE, dirname);
        goto badexit;
    }
    xfree(dirpatt);
    do{
        if (mmp_list_add_data(ret, xstrdup(FindFileData.cFileName))
                                                                !=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            goto badexit;
        }
    }while(FindNextFile(hFind, &FindFileData));
    if (FindClose(hFind)==0) {
        mmp_setError_ext(MMP_ERR_FILE, dirname);
        goto badexit;
    }
#endif
    return ret;
badexit:
    if (ret!=NULL)
        mmp_dir_free_list_results(&ret);
    return NULL;
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_listfiles(void)
{
    return MMP_TAP_UNTESTED;
}
static t_mmp_tap_result_e test_freelist(void)
{
    return MMP_TAP_UNTESTED;
}
ret_t mmp_dir_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret = mmp_tap_test(cycle, "mmp_dir_listfiles", "unimplemented",
                                                test_listfiles()))!=MMP_ERR_OK)
        return ret;
    if ((ret = mmp_tap_test(cycle, "mmp_dir_free_list_results","unimplemented",
                                                test_freelist()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
