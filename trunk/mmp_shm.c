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
#include "mmp_shm.h"

/* detach the shared memory from the current thread */
void mmp_shm_detach(t_mmp_shm_s **map)
{
    if (map==NULL || *map==NULL) return;
#ifdef _WIN32
    if ((*map)->base!=NULL)
        (void)UnmapViewOfFile((*map)->base);
    if ((*map)->map)
        (void)CloseHandle((*map)->map);
#else
    if ((*map)->base!=NULL)
        (void)munmap((*map)->base, (*map)->len);
    if ((*map)->filename!=NULL)
        (void)shm_unlink((*map)->filename);
#endif
    if ((*map)->filename!=NULL)
        xfree((*map)->filename);
    xfree(*map);
    *map = NULL;
}

/* attach a shared memory to the current process */
t_mmp_shm_s *mmp_shm_attach(const char *name, size_t size)
{
    t_mmp_shm_s *ret;
#ifdef _WIN32
    int is_first = 0;
#endif
    if (name==NULL || *name=='\0' || size<=0) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    if ((ret->filename = xstrdup(name))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        goto badexit;
    }
    ret->len = size;
#ifdef _WIN32
    if ((ret->map = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL,
                                    PAGE_READWRITE, 0, size, name))==NULL) {
        mmp_setError(MMP_ERR_SHM);
        goto badexit;
    }
    is_first = (GetLastError()!=ERROR_ALREADY_EXISTS);
    if ((ret->base = MapViewOfFile(ret->map, FILE_MAP_WRITE,
                                                            0, 0, 0))==NULL) {
        mmp_setError(MMP_ERR_SHM);
        goto badexit;
    }
    /* if we are the first process, clean the memory */
    if (is_first==1)
        memset(ret->base, 0, ret->len);
#else
    if ((ret->map = shm_open(name, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR))<0) {
        mmp_setError(MMP_ERR_SHM);
        goto badexit;
    }
    if (ftruncate(ret->map, ret->len)==-1) {
        mmp_setError(MMP_ERR_SHM);
        goto badexit;
    }
    if ((ret->base = mmap(NULL, ret->len, PROT_READ | PROT_WRITE, MAP_SHARED,
                                                ret->map, 0))==MAP_FAILED) {
        mmp_setError(MMP_ERR_SHM);
        goto badexit;
    }
#endif
    return ret;
badexit:
    mmp_shm_detach(&ret);
    return NULL;
}

static int is_shm_valid(t_mmp_shm_s *shm)
{
#ifdef _WIN32
    if (shm==NULL || shm->base==NULL || shm->len<=0 || shm->map==NULL)
#else
    if (shm==NULL || shm->base==NULL || shm->len<=0 || shm->map<0)
#endif
        return 0;
    return 1;
}

/* read from the shared memory */
ret_t mmp_shm_getdata(t_mmp_shm_s *shm, void *dst, size_t from, size_t len)
{
    if (!is_shm_valid(shm) || dst==NULL || len==0) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((from+len)>=shm->len) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    memcpy(dst, ((unsigned char *)shm->base)+from, len);
    return MMP_ERR_OK;
}

/* write to the shared memory */
ret_t mmp_shm_putdata(t_mmp_shm_s *shm, void *src, size_t from, size_t len)
{
    if (!is_shm_valid(shm) || src==NULL || len==0) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((from+len)>=shm->len) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    memcpy(((unsigned char *)shm->base)+from, src, len);
    return MMP_ERR_OK;
}

