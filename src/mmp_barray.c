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
#include "mmp_barray.h"

/** internal barray record */
typedef struct mmp_barray_rec_s {
    int present:1;                                  /* record present? */
    unsigned int idx:(sizeof(t_mmp_barray_idx)*8-1);/* record index */
    /* data follows */
} t_mmp_barray_rec_s;

/** internal barray page */
typedef struct mmp_barray_page_s {
    t_mmp_mmap_s *records;                          /* ptr to records */
    int dirty:1;                                    /* page dirty? */
    unsigned int n_data:(sizeof(uint16_t)*8-1);     /* number of records */
    t_mmp_barray_idx start;                         /* first index */
    t_mmp_barray_idx end;                           /* last index */
} t_mmp_barray_page_s;

/** internal cache hit count */
typedef struct mmp_barray_hitcache_s {
    unsigned int page_n;                            /* page number */
    unsigned int hit_count;                         /* hit count */
} t_mmp_barray_hitcache_s;

/** barray struct */
struct mmp_barray_s {
    int fd;                                         /* underlaying file */
    t_mmp_barray_page_s **pages;                    /* pages */
    unsigned int n_pages;                           /* number of pages */
    unsigned int page_len;                          /* page length */
    unsigned int data_size;                         /* record data size */
    unsigned int rec_size;                          /* record size */
    unsigned int recs_per_page;                     /* records per page */
    t_mmp_barray_hitcache_s *map_cache;             /* mmap cache */
    unsigned int map_cache_n;                       /* # of cache entries */
    unsigned int max_map_cache_n;                   /* max mmap entries */
};

#define MMP_BARRAY_INVALID_IDX ((t_mmp_barray_idx)(-1))
#define MMP_BARRAY_LASTPAGE(_B) ((_B)->pages[(_B)->n_pages-1])
#define MMP_BARRAY_RECORD(_BARRAY, _PAGE, _IDX) ( \
    (t_mmp_barray_rec_s*) \
        (((unsigned char*)(_PAGE)->records->ptr)+(_IDX)*(_BARRAY)->rec_size) )

static t_mmp_barray_page_s *create_page(t_mmp_barray_s *barray)
{
    t_mmp_barray_page_s *ret;
    MMP_CHECK_OR_RETURN((barray!=NULL), NULL);
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->dirty = 1;
    ret->n_data = 0;
    ret->start = ret->end = MMP_BARRAY_INVALID_IDX;
    ret->records = NULL;
    return ret;
}

static void destroy_page(t_mmp_barray_page_s **page)
{
    if (page==NULL || *page==NULL) return;
    if ((*page)->records!=NULL)
        mmp_munmap(&(*page)->records);
    MMP_XFREE_AND_NULL(*page);
}

static t_mmp_barray_s *create_barray(unsigned int page_size,
                                        unsigned int data_size,
                                        unsigned int max_cache_entries)
{
    t_mmp_barray_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->map_cache = xmalloc(sizeof(*ret->map_cache)*max_cache_entries);
    if (ret->map_cache==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(ret);
        return NULL;
    }
    ret->fd = -1;
    ret->pages = NULL;
    ret->n_pages = 0;
    ret->page_len = page_size;
    ret->data_size = data_size;
    ret->rec_size = sizeof(t_mmp_barray_rec_s)+data_size;
    ret->recs_per_page = ret->page_len/ret->rec_size;
    ret->map_cache_n = 0;
    ret->max_map_cache_n = max_cache_entries;
    return ret;
}

static void destroy_barray(t_mmp_barray_s **barray)
{
    unsigned int i;
    if (barray==NULL || *barray==NULL) return;
    if ((*barray)->pages)
        for (i=0; i<(*barray)->n_pages; ++i)
            if ((*barray)->pages[i]!=NULL)
                destroy_page(&(*barray)->pages[i]);
    if ((*barray)->fd>=0)
        mmp_close((*barray)->fd);
    if ((*barray)->map_cache!=NULL)
        xfree((*barray)->map_cache);
    MMP_XFREE_AND_NULL(*barray);
}

void mmp_barray_destroy(t_mmp_barray_s **barray)
{
    if (barray==NULL || *barray==NULL) return;
    destroy_barray(barray);
}

/** \test test_barray */
t_mmp_barray_s *mmp_barray_create(const char *fname, unsigned int page_size,
                                    unsigned int data_size,
                                    unsigned int max_cache_entries)
{
    t_mmp_barray_s *ret;
    t_mmp_barray_page_s *page;
    t_mmp_barray_rec_s *rec_p;
    t_mmp_stat_s sb;
    unsigned int i, j;
    MMP_CHECK_OR_RETURN((fname!=NULL && *fname!='\0' && max_cache_entries>0),
                                                                        NULL);
    if ((ret = create_barray(page_size, data_size, max_cache_entries))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    if (    ((ret->fd = mmp_open(fname, O_RDWR|O_CREAT, 0666))<0) ||
            (mmp_fstat(ret->fd, &sb)==-1)    ) {
        mmp_setError(MMP_ERR_FILE);
        goto badexit;
    }
    ret->n_pages = sb.st_size / ret->page_len;
    if (ret->n_pages>0) {
        if ((ret->pages = xmalloc(sizeof(*ret->pages)*ret->n_pages))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            goto badexit;
        }
        for (i=0; i<ret->n_pages; ++i) {
            if ((page = xmalloc(sizeof(*page)))==NULL) {
                mmp_setError(MMP_ERR_ENOMEM);
                goto badexit;
            }
            if ((page->records = mmp_mmap(NULL, ret->page_len, PROT_READ,
                        MAP_PRIVATE, ret->fd, i*ret->page_len))==MAP_FAILED) {
                mmp_setError(MMP_ERR_FILE);
                goto badexit;
            }
            page->n_data = 0;
            page->start = page->end = MMP_BARRAY_INVALID_IDX;
            for (j=0; j<ret->recs_per_page; ++j) {
                rec_p = MMP_BARRAY_RECORD(ret, page, j);
                if (rec_p->present==0) continue;
                ++page->n_data;
                if (page->start==MMP_BARRAY_INVALID_IDX)
                    page->start = rec_p->idx;
                page->end = rec_p->idx;
            }
            mmp_munmap(&page->records);
            page->dirty = 0;
            ret->pages[i] = page;
        }
    }
    return ret;
badexit:
    destroy_barray(&ret);
    return NULL;
}

static ret_t insert_new_page(t_mmp_barray_s *barray)
{
    t_mmp_barray_page_s *page, **rll;
    MMP_CHECK_OR_RETURN((barray!=NULL), MMP_ERR_PARAMS);
    if ((page = create_page(barray))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    ++barray->n_pages;
    if ((rll = xrealloc(barray->pages, sizeof(*rll)*barray->n_pages))==NULL) {
        --barray->n_pages;
        destroy_page(&page);
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    barray->pages = rll;
    MMP_BARRAY_LASTPAGE(barray) = page;
    if (mmp_pwrite(barray->fd, "\0", 1,
                                    barray->n_pages*barray->page_len-1)!=1) {
        destroy_page(&page);
        --barray->n_pages;
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    return MMP_ERR_OK;
}

/* ************************** map cache functions ************************** */

static int is_map_limit_ok(t_mmp_barray_s *barray)
{
    if (barray->map_cache_n<barray->max_map_cache_n)
        return 1;
    return 0;
}
static unsigned int get_cache_idx_from_page(t_mmp_barray_s *barray,
                                            unsigned int page_n)
{
    unsigned int i;
    for (i=0; i<barray->map_cache_n; ++i)
        if (barray->map_cache[i].page_n == page_n)
            return i;
    return UINT_MAX;
}
static int is_cached(t_mmp_barray_s *barray, unsigned int page_n)
{
    if (get_cache_idx_from_page(barray, page_n)!=UINT_MAX)
            return 1;
    return 0;
}
static ret_t cache_get_less_hit(t_mmp_barray_s *barray)
{
    unsigned int i, lidx=0, min = UINT_MAX;
    for (i=0; i<barray->map_cache_n; ++i)
        if (barray->map_cache[i].hit_count<min) {
            min = barray->map_cache[i].hit_count;
            lidx = i;
        }
    return barray->map_cache[lidx].page_n;
}
static ret_t cache_remove(t_mmp_barray_s *barray, unsigned int page_n)
{
    unsigned int idx, i;
    if ((idx = get_cache_idx_from_page(barray, page_n))==UINT_MAX)
        return MMP_ERR_OK;
    for (i=idx; i<barray->map_cache_n-1; ++i) {
        barray->map_cache[i].page_n = barray->map_cache[i+1].page_n;
        barray->map_cache[i].hit_count = barray->map_cache[i+1].hit_count;
    }
    --barray->map_cache_n;
    return MMP_ERR_OK;
}
static ret_t cache_hit(t_mmp_barray_s *barray, unsigned int page_n)
{
    unsigned int idx;
    if ((idx = get_cache_idx_from_page(barray, page_n))==UINT_MAX) {
        idx = barray->map_cache_n++;
        barray->map_cache[idx].page_n = page_n;
        barray->map_cache[idx].hit_count = 0;
    }
    ++barray->map_cache[idx].hit_count;
    return MMP_ERR_OK;
}

/* *********************** end of map cache functions ********************** */

static ret_t map_page(t_mmp_barray_s *barray, unsigned int page_n)
{
    MMP_CHECK_OR_RETURN((barray!=NULL && barray->pages!=NULL &&
                        barray->n_pages>page_n && barray->pages[page_n]!=NULL),
                        MMP_ERR_PARAMS);
    if (!is_map_limit_ok(barray) && !is_cached(barray, page_n)) {
        unsigned int idx;
        idx = cache_get_less_hit(barray);
        cache_remove(barray, idx);
        mmp_munmap(&barray->pages[idx]->records);
    }
    if ((barray->pages[page_n]->records = mmp_mmap(NULL, barray->page_len,
            PROT_READ|PROT_WRITE, MAP_SHARED, barray->fd,
            page_n*barray->page_len))==MAP_FAILED) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    cache_hit(barray, page_n);
    return MMP_ERR_OK;
}

/** \test test_barray */
ret_t mmp_barray_insert(t_mmp_barray_s * __restrict barray,
                        t_mmp_barray_idx idx, const void * __restrict data)
{
    t_mmp_barray_page_s *page;
    t_mmp_barray_rec_s *rec_p;
    ret_t i_ret;
    MMP_CHECK_OR_RETURN((barray!=NULL && data!=NULL), MMP_ERR_PARAMS);
    if (    (barray->n_pages==0) || (barray->pages==NULL) ||
            (MMP_BARRAY_LASTPAGE(barray)==NULL) ||
            (MMP_BARRAY_LASTPAGE(barray)->n_data>=barray->recs_per_page) ) {
        if ((i_ret = insert_new_page(barray))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    }
    page = MMP_BARRAY_LASTPAGE(barray);
    MMP_CHECK_OR_RETURN((idx>page->end || page->end==MMP_BARRAY_INVALID_IDX),
                        MMP_ERR_PARAMS);
    if (page->records==NULL)
        if ((i_ret = map_page(barray, barray->n_pages-1))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    rec_p = MMP_BARRAY_RECORD(barray, page, page->n_data);
    rec_p->idx = idx;
    memcpy(rec_p+1, data, barray->data_size);
    rec_p->present = 1;
    page->dirty = 1;
    page->end = idx;
    if (page->start==MMP_BARRAY_INVALID_IDX)
        page->start = idx;
    ++page->n_data;
    return MMP_ERR_OK;
}

static unsigned int search_page(t_mmp_barray_idx idx, t_mmp_barray_s *barray)
{
    unsigned int beg, mid, end;
    if (barray->n_pages==0)
        return UINT_MAX;
    beg = 0;
    end = barray->n_pages;
    while (beg<=end && end!=MMP_BARRAY_INVALID_IDX) {
        mid = (end+beg)/2;
        if (idx<barray->pages[mid]->start) end=mid-1;
        else if (idx>barray->pages[mid]->end) beg=mid+1;
        else return mid;
    }
    return beg;
}

static t_mmp_barray_rec_s *search_in_page(t_mmp_barray_idx idx,
                                   unsigned int page_n, t_mmp_barray_s *barray)
{
    t_mmp_barray_idx beg, mid, end;
    t_mmp_barray_page_s *page;
    t_mmp_barray_rec_s *rec_p;
    ret_t i_ret;
    page = barray->pages[page_n];
    if (idx>page->end || idx<page->start)
        return NULL;
    if (page->records==NULL)
        if ((i_ret = map_page(barray, page_n))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return NULL;
        }
    beg = 0;
    end = page->n_data-1;
    while (beg!=end) {
        mid = (end+beg)/2;
        rec_p = MMP_BARRAY_RECORD(barray, page, mid);
        if (idx<rec_p->idx) end = mid-1;
        else if (idx>rec_p->idx) beg = mid+1;
        else return rec_p;
    }
    rec_p = MMP_BARRAY_RECORD(barray, page, beg);
    if (rec_p->idx==idx)
        return rec_p;
    return NULL;
}

/** \test test_barray */
ret_t mmp_barray_search(t_mmp_barray_s * __restrict barray,
                        t_mmp_barray_idx idx, void ** __restrict data)
{
    unsigned int page_n;
    t_mmp_barray_rec_s *rec;
    MMP_CHECK_OR_RETURN((barray!=NULL && (*data)!=NULL), MMP_ERR_PARAMS);
    if (    ((page_n = search_page(idx, barray))==UINT_MAX) ||
            ((rec = search_in_page(idx, page_n, barray))==NULL) ||
            (rec->present==0) ) {
        *data = NULL;
        return MMP_ERR_OK;
    }
    *data = rec+1;
    return MMP_ERR_OK;
}

ret_t mmp_barray_delete(t_mmp_barray_s *barray, t_mmp_barray_idx idx)
{
    unsigned int page_n;
    t_mmp_barray_rec_s *rec;
    if (
            ((page_n = search_page(idx, barray))==UINT_MAX) ||
            ((rec = search_in_page(idx, page_n, barray))==NULL) ||
            (rec->present==0) )
        return MMP_ERR_GENERIC;
    rec->present = 0;
    barray->pages[page_n]->dirty = 1;
    return MMP_ERR_OK;
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_barray(void)
{
    t_mmp_barray_s *barray;
    int i, *dt;
    void *pdt;
    remove("test.ba");
    /* first write and insert */
    if ((barray = mmp_barray_create("test.ba", mmp_system_getPageAlignment(),
                                                        sizeof(int), 20))==NULL)
        return MMP_TAP_FAILED;
    for (i=0; i<100000; ++i)
        if (mmp_barray_insert(barray, i+1, &i)!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return MMP_TAP_FAILED;
        }
    mmp_barray_destroy(&barray);
    if (barray!=NULL)
        return MMP_TAP_FAILED;
    /* re-read and search */
    if ((barray = mmp_barray_create("test.ba", mmp_system_getPageAlignment(),
                                                        sizeof(int), 20))==NULL)
        return MMP_TAP_FAILED;
    if (mmp_barray_search(barray, 2, &pdt)!=MMP_ERR_OK)
        return MMP_TAP_FAILED;
    dt = (int*)pdt;
    if (dt==NULL || *dt!=1) return MMP_TAP_FAILED;
    if (mmp_barray_search(barray, 23452, &pdt)!=MMP_ERR_OK)
        return MMP_TAP_FAILED;
    dt = (int*)pdt;
    if (dt==NULL || *dt!=23451) return MMP_TAP_FAILED;
    mmp_barray_destroy(&barray);
    if (barray!=NULL)
        return MMP_TAP_FAILED;
    /* clean */
    remove("test.ba");
    return MMP_TAP_PASSED;
}

ret_t mmp_barray_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "barray test", NULL,
                                        test_barray()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
