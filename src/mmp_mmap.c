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
#include "mmp_mmap.h"

/** \todo this is WRONG! */
t_mmp_mmap_s *mmp_mmap(void *ptr, size_t size, int prot, int flags, int fd,
                                                                long offset)
{
    t_mmp_mmap_s *ret;
#ifdef _WIN32
    HANDLE hfile;
#endif
    MMP_XMALLOC_OR_RETURN(ret, NULL);
#ifndef _WIN32
    if ((ret->ptr = mmap(ptr, size, prot, flags, fd, offset))==NULL) {
        xfree(ret);
        return NULL;
    }
#else
    ret->hmap = NULL;
    ret->ptr = NULL;
    if (    ((hfile = (HANDLE)_get_osfhandle(fd))==INVALID_HANDLE_VALUE) ||
            ((ret->hmap = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, size+offset, NULL))==NULL) ||
            ((ret->ptr = MapViewOfFile(ret->hmap, FILE_MAP_WRITE, 0, offset, size))==NULL)
            ) {
        if (ret->hmap!=NULL)
            CloseHandle(ret->hmap);
        xfree(ret);
        return NULL;
    }
#endif
    ret->length = size;
    return ret;
}

/** \todo this is WRONG! */
int mmp_munmap(t_mmp_mmap_s **map)
{
    int ret = 0;
    if (map==NULL || *map==NULL) return 0;
#ifndef _WIN32
    ret = munmap((*map)->ptr, (*map)->length);
#else
    if ((*map)->hmap!=NULL)
        CloseHandle((*map)->hmap);
    if ((*map)->ptr!=NULL)
        ret = UnmapViewOfFile((*map)->ptr);
#endif
    free(*map);
    *map = NULL;
    return ret;
}

/*
typedef struct area_s {
    size_t file_offset;
    void *ptr;
    size_t len;
} t_area_s;
typedef struct map_s {
    int fd;
    t_mmp_list_s *areas;
} t_map_s;

#define AINIT(_A)   ((_A)->file_offset)
#define AEND(_A)    ((_A)->file_offset+(_A)->len)

static void adjust_prot_flags(int *prot, int *flags)
{
    // TODO adjust prot and flags based on system
}

static ret_t map_area(t_area_s *area, int prot, int flags, int fd)
{
    MMP_CHECK_OR_RETURN((area!=NULL && area->ptr==NULL && fd>=0),
                        MMP_ERR_PARAMS);
    adjust_prot_flags(&prot, &flags);
    area->ptr = mmap(NULL, area->len, prot, flags, fd, area->file_offset);
    if (area->ptr==NULL) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    return MMP_ERR_OK;
}

static ret_t unmap_area(t_area_s *area)
{
    MMP_CHECK_OR_RETURN((area!=NULL && area->ptr!=NULL), MMP_ERR_PARAMS);
    if (munmap(area->ptr, area->len)!=0)
        return MMP_ERR_FILE;
    return MMP_ERR_OK;
}

static t_map_s *create_map(void)
{
    t_map_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    if ((ret->areas = mmp_list_create())==NULL) {
        xfree(ret);
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    return ret;
}

static void destroy_area_unmapping(t_area_s **area)
{
    unmap_area(*area);
    destroy_area_s(area);
}
static void destroy_area_unmapping_v(void **area)
{
    destroy_area_unmapping((t_area_s**)area);
}

static void destroy_map(t_map_s **map)
{
    if (map==NULL || *map==NULL) return;
    if ((*map)->areas)
        mmp_list_destroy_withdata((*map)->areas, destroy_area_unmapping_v);
    MMP_XFREE_AND_NULL(*map);
}

static ret_t insert_and_map_area(t_map_s *map, t_area_s **area, int prot, int flags)
{
    MMP_CHECK_OR_RETURN((map!=NULL && area!=NULL && *area!=NULL && map->fd>=0),
                        MMP_ERR_PARAMS);
    if (insert_area(*map, *area)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        destroy_area_s(area);
        return MMP_ERR_GENERIC;
    }
    if (map_area(*area, prot, flags, map->fd)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        remove_destroy_area((*map)->areas, area);
        return MMP_ERR_GENERIC;
    }
    return MMP_ERR_OK;
}

ret_t mmp_unmap(t_map_s **map, void *ptr, size_t length)
{
    t_area_s *area;
    MMP_CHECK_OR_RETURN((map!=NULL && *map!=NULL && ptr!=NULL && length!=0),
                        MMP_ERR_PARAMS);
    area = search_area_by_ptr(map, ptr);
    if (area==NULL || ptr+length>AEND(area)) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    if (area->ptr==ptr && area->len==length) {
        if (unmap_area(area)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
        remove_destroy_area((*map)->areas, &area);
    } else {
        // TODO umap partial area
    }
    return MMP_ERR_OK;
}

void *mmp_mmap(t_map_s **map, size_t length, int prot, int flags, int fd, size_t offset)
{
    t_area_s *nu, *pr;
    size_t noff, nlen;
    MMP_CHECK_OR_RETURN((length!=0 && fd>=0), NULL);
    if (*map==NULL)
        if ((*map = create_map())==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    (*map)->fd = fd;
    noff = offset;
    nlen = length;
    adjust_area(&noff, &nlen);
    if ((nu = create_area_s(NULL, noff, nlen))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    pr = search_area(*map, nu);
    if (pr==NULL) {
        if (insert_and_map_area(map, &nu, prot, flags)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            remove_destroy_area((*map)->areas, &nu);
            return NULL;
        }
    } else {
        if (AINIT(nu)==AINIT(pr) && AEND(nu)==AEND(pr)) {
            destroy_area_s(&nu);
            nu = pr;
        } else {
            if (unmap_area(pr)!=MMP_ERR_OK) {
                mmp_setError(MMP_ERR_GENERIC);
                destroy_area_s(&nu);
                return NULL;
            }
            check_enlarge_area(nu, pr, &noff, &nlen);
            remove_destroy_area((*map)->areas, &pr);
            if (insert_and_map_area(map, &nu, prot, flags)!=MMP_ERR_OK) {
                mmp_setError(MMP_ERR_GENERIC);
                remove_destroy_area((*map)->areas, &nu);
                return NULL;
            }
        }
    }
    return pr->ptr+(offset-noff);
}

static void check_enlarge_area(t_area_s *nu, t_area_s *a2, size_t *offset, size_t *len)
{
    if (AINIT(a2)<AINIT(nu))
        nu->file_offset = *offset = AINIT(a2);
    if (AEND(nu)<AEND(a2))
        nu->len = *len = AEND(a2)-AINIT(nu);
}

static int area_comparer(const t_area_s *a1, const t_area_s *a2)
{
    if (AINIT(a2)>AEND(a1)) return -1;
    if (AINIT(a1)>AEND(a2)) return 1;
    return 0;
}

static int area_comparer_v(const void *a1, const void *a2)
{
    return area_comparer((const t_area_s*)a1, (const t_area_s*)a2);
}

static ret_t insert_area(t_map_s *map, t_area_s *area)
{
    return mmp_list_insert_data_sorted(map->areas, area, area_comparer_v);
}

static void remove_destroy_area(t_map_s *map, t_area_s **area)
{
    mmp_list_del_elem_by_data(map->areas, *area);
    destroy_area_s(area);
}

static t_area_s *search_area(t_map_s *map, t_area_s *area)
{
    return mmp_list_find_data_sorted(map->areas, area, area_comparer_v);
}

static int area_ptr_comparer(void *ptr, t_area_s *area)
{
    if (area->ptr>=ptr && (area->ptr+area->len)<ptr)
        return 0;
    return -1;
}

static int area_ptr_comparer_v(void *ptr, void *area)
{
    return area_ptr_comparer(ptr, (t_area_s*)area);
}

static t_area_s *search_area_by_ptr(t_map_s *map, void *ptr)
{
    t_mmp_listelem_s *el;
    if ((el = mmp_list_find_data_lambda(map, ptr, area_ptr_comparer_v))==NULL)
        return NULL;
    return (t_area_s*)el->data;
}

static void adjust_area(size_t *offset, size_t *len)
{
    // TODO get the area adjustments based on system
}

static t_area_s *create_area_s(void *ptr, size_t offset, size_t len)
{
    t_area_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->offset = offset;
    ret->ptr = ptr;
    ret->len = len;
    return ret;
}

static void destroy_area_s(t_area_s **area)
{
    if (area==NULL || *area==NULL) return;
    MMP_XFREE_AND_NULL(*area);
}
 */
