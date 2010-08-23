#include "mmp_barray.h"

#define LASTPAGE(_B) ((_B)->pages[(_B)->npages-1])

static t_mmp_barray_page_s *create_page(t_mmp_barray_s *barray)
{
    t_mmp_barray_page_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    page->d = 1;
    page->n_data = 0;
    page->start = page->end = -1;
    page->rec = NULL;
    return ret;
}

static void destroy_page(t_mmp_barray_page_s **page, t_mmp_barray_s *barray)
{
    if (page==NULL || *page==NULL) return;
    if ((*page)->records!=NULL)
        munmap((*page)->records, barray->page_len);
    xfree(*page);
    *page = NULL;
}

static t_mmp_barray_s *create_barray(void)
{
    t_mmp_barray_s *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL) {
        set_mmpError(MMP_ERR_ENOMEM);
        return NULL;
    }
    ret->fd = -1;
    ret->pages = NULL;
    ret->n_pages = 0;
    ret->page_len = sysconf(_SC_PAGESIZE);
    ret->recs_per_page = ret->page_len/sizeof(*ret->pages);
    return ret;
}

static destroy_barray(t_mmp_barray_s **barray)
{
    unsigned int i;
    if (barray==NULL || *barray==NULL) return;
    if ((*barray)->pages)
        for (i=0; i<(*barray)->n_pages; ++i)
            if ((*barray)->pages[i]!=NULL)
                destroy_page(&(*barray)->pages[i]);
    if ((*barray)->fd>=0)
        close((*barray)->fd);
    xfree(*barray);
    *barray = NULL;
}

void mmp_barray_destroy(t_mmp_barray_s **barray)
{
    destroy_barray(barray);
}

t_mmp_barray_s *mmp_barray_create(const char *fname)
{
    t_mmp_barray_s *ret;
    t_mmp_barray_page_s *page;
    t_mmp_barray_rec_s *rec;
    struct stat sb;
    unsigned int i, j;
    if ((ret = create_barray())==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    if (    ((ret->fd = open(fname, O_RDWR|O_CREAT, 0666))<0) ||
            (fstat(ret->fd, &sb)==-1)    ) {
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
                goto bad_exit;
            }
            if ((rec = mmap(NULL, ret->page_len, PROT_READ, MAP_PRIVATE,
                                    ret->fd, i*ret->page_len))==MAP_FAILED) {
                mmp_setError(MMP_ERR_FILE);
                goto bad_exit;
            }
            page->n_data = 0;
            page->start = page->end = -1;
            for (j=0; j<ret->recs_per_page; ++j) {
                if (rec[j].p==0) continue;
                ++page->n_data;
                if (page->start==-1)
                    page->start = rec[j].idx;
                page->end = rec[j].idx;
            }
            munmap(rec, ret->page_len);
            page->records = NULL;
            page->d = 0;
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
    if ((barray->pages[page_n]->records = mmap(NULL, barray->page_len,
            PROT_READ|PROT_WRITE, MAP_SHARED, barray->fd,
            page_n*barray->page_len))==MAP_FAILED) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    return MMP_ERR_OK;
}

ret_t mmp_barray_insert(t_mmp_barray_s *barray, t_mmp_barray_idx idx,
                        void *data)
{
    t_mmp_barray_page_s *page;
    ret_t i_ret;
    if (    (barray->n_pages==0) ||
            (LASTPAGE(barray)->n_data>=barray->recs_per_page) ) {
        if ((i_ret = insert_new_page(barray))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    }
    page = LASTPAGE(barray);
    if ((idx<=page->end) && (page->end!=-1)) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if (page->records==NULL)
        if ((i_ret = map_page(barray, barray->n_pages-1))!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_GENERIC);
            return i_ret;
        }
    page->records[page->n_data].idx = idx;
    memcpy(page->records[pages->n_data].data, data, sizeof(data));
    page->records[page->n_data].p = 1;
    page->d = 1;
    page->end = idx;
    if (page->start==-1)
        page->start = idx;
    ++page->n_data;
    return MMP_ERR_OK;
}

static unsigned int search_page(t_mmp_barray_idx idx, t_mmp_barray_s *barray)
{
    unsigned int beg, mid, end;
    if (barray->n_pages==0)
        return -1;
    beg = 0;
    end = barray->n_pages;
    while (beg<=end && end!=-1) {
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
            return i_ret;
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
                        void *data)
{
    unsigned int page_n;
    t_mmp_barray_rec_s *rec;
    if (
            ((page_n = search_page(idx, barray))==-1) ||
            ((rec = search_in_page(idx, page_n, barray))==NULL) ||
            (rec->p==0) )
        return MMP_ERR_GENERIC;
    memcpy(data, &rec->data, sizeof(data));
    return MMP_ERR_OK;
}

ret_t mmp_barray_delete(t_mmp_barray_s *barray, t_mmp_barray_idx idx)
{
    unsigned int page_n;
    t_mmp_barray_rec_s *rec;
    if (
            ((page_n = search_page(idx, barray))==-1) ||
            ((rec = search_in_page(idx, page_n, barray))==NULL) ||
            (rec->p==0) )
        return MMP_ERR_GENERIC;
    rec->p = 0;
    barray->pages[page_n]->d = 1;
    return MMP_ERR_OK;
}

