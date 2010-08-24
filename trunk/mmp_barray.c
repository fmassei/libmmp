#include "mmp_barray.h"

#define MMP_BARRAY_INVALID_IDX ((t_mmp_barray_idx)(-1))
#define LASTPAGE(_B) ((_B)->pages[(_B)->n_pages-1])

static t_mmp_barray_page_s *create_page(t_mmp_barray_s *barray)
{
    t_mmp_barray_page_s *ret;
    if (barray==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->dirty = 1;
    ret->n_data = 0;
    ret->start = ret->end = MMP_BARRAY_INVALID_IDX;
    ret->records = NULL;
    return ret;
}

static void destroy_page(t_mmp_barray_page_s **page, t_mmp_barray_s *barray)
{
    if (page==NULL || *page==NULL) return;
    if ((*page)->records!=NULL && barray!=NULL)
        mmp_munmap((*page)->records, barray->page_len);
    xfree(*page);
    *page = NULL;
}

static t_mmp_barray_s *create_barray(unsigned int page_size,
                                        unsigned int data_size)
{
    t_mmp_barray_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->fd = -1;
    ret->pages = NULL;
    ret->n_pages = 0;
    ret->page_len = page_size;
    ret->data_size = data_size;
    ret->rec_size = sizeof(t_mmp_barray_rec_s)-sizeof(unsigned char*)+data_size;
    ret->recs_per_page = ret->page_len/ret->rec_size;
    return ret;
}

static void destroy_barray(t_mmp_barray_s **barray)
{
    unsigned int i;
    if (barray==NULL || *barray==NULL) return;
    if ((*barray)->pages)
        for (i=0; i<(*barray)->n_pages; ++i)
            if ((*barray)->pages[i]!=NULL)
                destroy_page(&(*barray)->pages[i], *barray);
    if ((*barray)->fd>=0)
        mmp_close((*barray)->fd);
    xfree(*barray);
    *barray = NULL;
}

void mmp_barray_destroy(t_mmp_barray_s **barray)
{
    if (barray==NULL || *barray==NULL) return;
    destroy_barray(barray);
}

t_mmp_barray_s *mmp_barray_create(const char *fname, unsigned int page_size,
                                    unsigned int data_size)
{
    t_mmp_barray_s *ret;
    t_mmp_barray_page_s *page;
    t_mmp_barray_rec_s *rec;
    t_mmp_stat_s sb;
    unsigned int i, j;
    if (fname==NULL || *fname=='\0') {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    if ((ret = create_barray(page_size, data_size))==NULL) {
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
            if ((rec = mmp_mmap(NULL, ret->page_len, PROT_READ, MAP_PRIVATE,
                                    ret->fd, i*ret->page_len))==MAP_FAILED) {
                mmp_setError(MMP_ERR_FILE);
                goto badexit;
            }
            page->n_data = 0;
            page->start = page->end = MMP_BARRAY_INVALID_IDX;
            for (j=0; j<ret->recs_per_page; ++j) {
                if (rec[j].present==0) continue;
                ++page->n_data;
                if (page->start==MMP_BARRAY_INVALID_IDX)
                    page->start = rec[j].idx;
                page->end = rec[j].idx;
            }
            mmp_munmap(rec, ret->page_len);
            page->records = NULL;
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
    if (barray==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((page = create_page(barray))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    ++barray->n_pages;
    if ((rll = xrealloc(barray->pages, sizeof(*rll)*barray->n_pages))==NULL) {
        --barray->n_pages;
        destroy_page(&page, barray);
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    barray->pages = rll;
    LASTPAGE(barray) = page;
    if (pwrite(barray->fd, "\0", 1, barray->n_pages*barray->page_len-1)!=1) {
        destroy_page(&page, barray);
        --barray->n_pages;
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    return MMP_ERR_OK;
}

static ret_t map_page(t_mmp_barray_s *barray, unsigned int page_n)
{
    if (barray==NULL || barray->pages==NULL || barray->n_pages<=page_n ||
            barray->pages[page_n]==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((barray->pages[page_n]->records = mmp_mmap(NULL, barray->page_len,
            PROT_READ|PROT_WRITE, MAP_SHARED, barray->fd,
            page_n*barray->page_len))==MAP_FAILED) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    return MMP_ERR_OK;
}

ret_t mmp_barray_insert(t_mmp_barray_s *barray, t_mmp_barray_idx idx,
                        unsigned char *data)
{
    t_mmp_barray_page_s *page;
    ret_t i_ret;
    if (barray==NULL || data==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (    (barray->n_pages==0) || (barray->pages==NULL) ||
            (LASTPAGE(barray)==NULL) ||
            (LASTPAGE(barray)->n_data>=barray->recs_per_page) ) {
        if ((i_ret = insert_new_page(barray))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    }
    page = LASTPAGE(barray);
    if ((idx<=page->end) && (page->end!=MMP_BARRAY_INVALID_IDX)) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (page->records==NULL)
        if ((i_ret = map_page(barray, barray->n_pages-1))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    page->records[page->n_data].idx = idx;
    memcpy(page->records[page->n_data].data, data, barray->data_size);
    page->records[page->n_data].present = 1;
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
        if (idx<page->records[mid].idx) end = mid-1;
        else if (idx>page->records[mid].idx) beg = mid+1;
        else return &page->records[mid];
    }
    if (page->records[beg].idx==idx)
        return &page->records[beg];
    return NULL;
}

ret_t mmp_barray_search(t_mmp_barray_s *barray, t_mmp_barray_idx idx,
                        unsigned char *data)
{
    unsigned int page_n;
    t_mmp_barray_rec_s *rec;
    if (barray==NULL || data==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (    ((page_n = search_page(idx, barray))==UINT_MAX) ||
            ((rec = search_in_page(idx, page_n, barray))==NULL) ||
            (rec->present==0) ) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    memcpy(data, rec->data, barray->data_size);
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
ret_t mmp_barray_unittest(t_mmp_tap_cycle_s *cycle)
{
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
