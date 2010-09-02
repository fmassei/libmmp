#include "mmp_btree.h"

typedef int t_mmp_btoff;

#define INVALID_OFF ((t_mmp_btoff)1)

typedef struct rec_s {
    t_mmp_btoff off;
    t_mmp_btkey key;
    t_mmp_btval val;
} t_rec_s;

typedef struct pg_s {
    t_rec_s *recs;          /* ptr to secondary storage after mmap() */
    t_mmp_mmap_s *mm;       /* mapped memory structure */
    int n_el;               /* number of present elements */
    struct pg_s *parent;    /* ptr to parent page */
    t_mmp_btoff off;        /* file offset */
} t_pg_s;

struct mmp_bt_s {
    int R;                  /* btree size (number of keys per page) */
    int pgsize;             /* page size */
    t_pg_s *root;           /* root page ptr */
    t_pg_s **pgoff;         /* array of present pages */
    int n_pgoff;            /* number of present pages */
    int fd;                 /* underlaying file */
};

static ret_t check_mapped(t_mmp_bt_s *bt, int n_mp, ...)
{
    va_list va;
    t_pg_s *pg;
    int i;
    va_start(va, n_mp);
    for (i=0; i<n_mp; ++i) {
        pg = va_arg(va, t_pg_s*);
        if (pg->recs==NULL) {
            if ((pg->mm = mmp_mmap(NULL, bt->pgsize, PROT_READ|PROT_WRITE,
                                MAP_SHARED, bt->fd, pg->off))==MAP_FAILED) {
                mmp_setError(MMP_ERR_FILE);
                return MMP_ERR_FILE;
            }
            pg->recs = pg->mm->ptr;
        }
    }
    va_end(va);
    return MMP_ERR_OK;
}

static ret_t initialize_page(t_mmp_bt_s *bt, t_pg_s *pg)
{
    int i;
    MMP_CHECK_OR_RETURN((bt!=NULL && pg!=NULL), MMP_ERR_PARAMS);
    if (check_mapped(bt, 1, pg)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    for (i=0; i<bt->R+1; ++i)
        pg->recs[i].off = INVALID_OFF;
    return MMP_ERR_OK;
}

static ret_t insert_pg_into_bt(t_mmp_bt_s *bt, t_pg_s *pg)
{
    t_pg_s **npgoff;
    MMP_CHECK_OR_RETURN((bt!=NULL && pg!=NULL), MMP_ERR_PARAMS);
    if (mmp_pwrite(bt->fd, "\0", 1, (bt->n_pgoff+1)*bt->pgsize-1)!=1) {
        mmp_setError(MMP_ERR_FILE);
        return MMP_ERR_FILE;
    }
    npgoff = xrealloc(bt->pgoff, (bt->n_pgoff+1)*sizeof(*bt->pgoff));
    if (npgoff==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    bt->pgoff = npgoff;
    pg->off = bt->n_pgoff*bt->pgsize;
    if (initialize_page(bt, pg)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    bt->pgoff[bt->n_pgoff] = pg;
    ++bt->n_pgoff;
    return MMP_ERR_OK;
}

static void destroy_pg(t_pg_s **pg)
{
    if (pg==NULL || *pg==NULL) return;
    if ((*pg)->recs!=NULL)
        mmp_munmap(&((*pg)->mm));
    xfree(*pg);
    *pg = NULL;
}

static t_pg_s *create_empty_pg(void)
{
    t_pg_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->recs = NULL;
    ret->n_el = 0;
    ret->parent = NULL;
    ret->off = INVALID_OFF;
    ret->mm = NULL;
    return ret;
}

static t_pg_s *create_and_insert_pg(t_mmp_bt_s *bt)
{
    t_pg_s *ret;
    MMP_CHECK_OR_RETURN((bt!=NULL), NULL);
    if ((ret = create_empty_pg())==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return NULL;
    }
    if (insert_pg_into_bt(bt, ret)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        destroy_pg(&ret);
        return NULL;
    }
    return ret;
}

static int get_R_from_pgsize(int pgsize)
{
    int ret;
    ret = (pgsize / sizeof(t_rec_s)) - 1;
    return ret;
}

/** \test test_bt */
void mmp_bt_destroy(t_mmp_bt_s **bt)
{
    int i;
    if (bt==NULL || *bt==NULL) return;
    if ((*bt)->pgoff!=NULL) {
        for (i=0; i<(*bt)->n_pgoff; ++i)
            destroy_pg(&((*bt)->pgoff[i]));
        xfree((*bt)->pgoff);
    }
    if ((*bt)->fd>=0)
        mmp_close((*bt)->fd);
    xfree(*bt);
    *bt = NULL;
}

/** \test test_bt */
t_mmp_bt_s *mmp_bt_create(const char *fname, int page_size)
{
    t_mmp_bt_s *ret;
    t_mmp_stat_s sb;
    MMP_CHECK_OR_RETURN((fname!=NULL && *fname!='\0' && page_size>0), NULL);
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    if (    ((ret->fd = mmp_open(fname, O_RDWR|O_CREAT, 0666))<0) ||
            (mmp_fstat(ret->fd, &sb)==-1)    ) {
        mmp_setError(MMP_ERR_FILE);
        goto badexit;
    }
    if (sb.st_size>0) {
        /* TODO */
    } else {
        ret->pgsize = page_size;
        ret->R = get_R_from_pgsize(page_size);
        ret->root = NULL;
        ret->pgoff = NULL;
        ret->n_pgoff = 0;
        if ((ret->root = create_and_insert_pg(ret))==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            goto badexit;
        }
    }
    return ret;
badexit:
    mmp_bt_destroy(&ret);
    return NULL;
}

static int off_to_idx(t_mmp_bt_s *bt, t_mmp_btoff off)
{
    return off/bt->pgsize;
}

static int is_off_valid(t_mmp_btoff off)
{
    return (off!=INVALID_OFF);
}

static t_pg_s *page_from_off(t_mmp_bt_s *bt, t_mmp_btoff off)
{
    if (!is_off_valid(off))
        return NULL;
    return bt->pgoff[off_to_idx(bt, off)];
}

/** \test test_bt */
ret_t mmp_bt_search(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval *val)
{
    t_pg_s *pg;
    t_mmp_btkey inkey;
    int i;
    MMP_CHECK_OR_RETURN((bt!=NULL && val!=NULL), MMP_ERR_PARAMS);
    pg = bt->root;
reloop:
    if (pg==NULL)
        return MMP_ERR_NOTFOUND;
    if (check_mapped(bt, 1, pg)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    for (i=0; i<pg->n_el; ++i) {
        inkey = pg->recs[i].key;
        if (inkey<key) {
            continue;
        } else if (inkey>key) {
            pg = page_from_off(bt, pg->recs[i].off);
            goto reloop;
        } else {
            memcpy(val, &pg->recs[i].val, sizeof(val));
            return MMP_ERR_OK;
        }
    }
    pg = page_from_off(bt, pg->recs[i].off);
    goto reloop;
}

static ret_t pg_insert_in_pos(t_pg_s *pg, t_mmp_btkey key,
                t_mmp_btval val, t_mmp_btoff off1, t_mmp_btoff off2, int pos)
{
    MMP_CHECK_OR_RETURN((pg!=NULL), MMP_ERR_PARAMS);
    memmove(pg->recs+pos+1, pg->recs+pos, sizeof(*pg->recs)*(pg->n_el-pos));
    pg->recs[pos].key = key;
    pg->recs[pos].val = val;
    pg->recs[pos].off = off1;
    pg->recs[pos+1].off = off2;
    ++pg->n_el;
    return MMP_ERR_OK;
}

static ret_t pg_insert(t_pg_s *pg, t_mmp_btkey key,
                        t_mmp_btval val, t_mmp_btoff off1, t_mmp_btoff off2)
{
    int i;
    t_mmp_btkey inkey;
    MMP_CHECK_OR_RETURN((pg!=NULL), MMP_ERR_PARAMS);
    for (i=0; i<pg->n_el; ++i) {
        inkey = pg->recs[i].key;
        if (inkey<key) {
            continue;
        } else if (inkey==key) {
            return MMP_ERR_PARAMS;
        } else {
            break; /* goto copy with right i value */
        }
    }
    return pg_insert_in_pos(pg, key, val, off1, off2, i);
}

static t_pg_s *pg_split(t_mmp_bt_s *bt, t_pg_s *pg)
{
    int sp;
    t_pg_s *par=NULL, *nu_bro=NULL, *old_root;
    par = pg->parent;
    if (pg->n_el<bt->R || (par!=NULL && par->n_el>=bt->R)) {
        mmp_setError(MMP_ERR_PARAMS);
        return NULL;
    }
    sp = pg->n_el/2;
    old_root = bt->root;
    if (par==NULL) { /* root split */
        if ((par = create_and_insert_pg(bt))==NULL) goto badexit;
        bt->root = par;
    }
    if ((nu_bro = create_and_insert_pg(bt))==NULL) goto badexit;
    if (check_mapped(bt, 3, pg, par, nu_bro)!=MMP_ERR_OK) goto badexit;
    pg_insert(par, pg->recs[sp].key, pg->recs[sp].val, pg->off,nu_bro->off);
    pg->parent = par;
    nu_bro->parent = par;
    memcpy(nu_bro->recs, pg->recs+sp+1, (pg->n_el-(sp+1))*sizeof(*pg->recs));
    nu_bro->n_el = pg->n_el-(sp+1);
    pg->n_el = sp;
    return par;
badexit:
    if (par!=NULL && bt->root==par) { /* tried to root-split, but failed */
        mmp_setError(MMP_ERR_GENERIC);
        destroy_pg(&par);
        bt->root = old_root;
    }
    if (nu_bro!=NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        destroy_pg(&nu_bro);
    }
    return NULL;
}

/** \test test_bt */
ret_t mmp_bt_insert(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval val)
{
    t_pg_s *pg;
    t_mmp_btkey inkey;
    t_mmp_btoff inoff;
    int i;
    MMP_CHECK_OR_RETURN((bt!=NULL), MMP_ERR_PARAMS);
    pg = bt->root;
reloop:
    if (pg->n_el>=bt->R)
        if ((pg = pg_split(bt, pg))==NULL) {
            mmp_setError(MMP_ERR_GENERIC);
            return MMP_ERR_GENERIC;
        }
    if (check_mapped(bt, 1, pg)!=MMP_ERR_OK) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    for (i=0; i<pg->n_el; ++i) {
        inkey = pg->recs[i].key;
        if (inkey<key) {
            continue;
        } else if (inkey==key) {
            return MMP_ERR_PARAMS;
        } else {
            inoff = pg->recs[i].off;
            if (is_off_valid(inoff)) {
                pg = page_from_off(bt, inoff);
                goto reloop;
            } else {
                /* leaf insert (inoff is INVALID_OFF) */
                return pg_insert_in_pos(pg, key, val, inoff, inoff, i); 
            }
        }
    }
    inoff = pg->recs[i].off;
    if (is_off_valid(inoff)) {
        pg = page_from_off(bt, inoff);
        goto reloop;
    } else {
        /* leaf insert (inoff is INVALID_OFF) */
        return pg_insert_in_pos(pg, key, val, inoff, inoff, i); 
    }
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_bt(void)
{
    t_mmp_bt_s *bt;
    int i, r, rv;
    remove("test.bt");
    if ((bt = mmp_bt_create("test.bt", mmp_system_getPageAlignment()))==NULL)
        return MMP_TAP_FAILED;
    for (i=0; i<100000; ++i) {
        if (mmp_bt_insert(bt, i, (i+1))!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return MMP_TAP_FAILED;
        }
    }
    for (i=0; i<100; ++i) {
        r = (int)(100000*(rand()/(RAND_MAX+1.0)));
        if (mmp_bt_search(bt, r, &rv)!=MMP_ERR_OK) return MMP_TAP_FAILED;
        if (rv!=r+1) return MMP_TAP_FAILED;
    }
    mmp_bt_destroy(&bt);
    if (bt!=NULL)
        return MMP_TAP_FAILED;
    remove("test.bt");
    return MMP_TAP_PASSED;
}
ret_t mmp_btree_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "btree functions", NULL,
                                        test_bt()))!=MMP_ERR_OK)
       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

