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

static ret_t insert_pg_in_bt(t_mmp_bt_s *bt, t_pg_s *pg)
{
    t_pg_s **npgoff;
    MMP_CHECK_OR_RETURN((bt!=NULL && pg!=NULL), MMP_ERR_PARAMS);
    npgoff = xrealloc(bt->pgoff, (bt->n_pgoff+1)*sizeof(*bt->pgoff));
    if (npgoff==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    bt->pgoff = npgoff;
    bt->pgoff[bt->n_pgoff] = pg;
    ++bt->n_pgoff;
    return MMP_ERR_OK;
}

static t_pg_s *create_empty_pg(void)
{
    t_pg_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->recs = NULL;
    ret->n_el = 0;
    ret->parent = NULL;
    ret->off = INVALID_OFF;
    return ret;
}

static int get_R_from_pgsize(int pgsize)
{
    int ret;
    ret = (pgsize / sizeof(t_rec_s)) - 1;
    return ret;
}

void mmp_bt_destroy(t_mmp_bt_s **bt)
{
    if (bt==NULL || *bt==NULL) return;
    xfree(*bt);
    *bt = NULL;
}

/** \todo missing unittest */
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

static ret_t check_mapped(int n_mp, ...)
{
    va_list va;
    va_start(va, n_mp);
    /* TODO */
    va_end(va);
    return MMP_ERR_OK;
}

/** \todo missing unittest */
ret_t mmp_bt_search(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval *val)
{
    t_pg_s *pg;
    t_mmp_btkey inkey;
    int i;
    pg = bt->root;
reloop:
    if (pg==NULL)
        return MMP_ERR_NOTFOUND;
    check_mapped(1, pg);
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
    /*pg->recs[pg->el+1].off = pg->recs[pg->el].off;
    for (j=pg->n_el; j>pos; --j) {
        pg->recs[j].key = pg->recs[j-1].key;
        pg->recs[j].off = pg->recs[j-1].off;
        pg->recs[j].val = pg->recs[j-1].val;
    }*/
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
    t_pg_s *par, *nu_bro;
    par = pg->parent;
    if (pg->n_el<bt->R || (par!=NULL && par->n_el>=bt->R))
        //return MMP_ERR_PARAMS;
        return NULL;
    sp = pg->n_el/2;
    if (par==NULL) { /* root split */
        par = create_empty_pg();
        bt->root = par;
    }
    nu_bro = create_empty_pg();
    check_mapped(3, pg, par, nu_bro);
    pg_insert(par, pg->recs[sp].key, pg->recs[sp].val, pg->off,nu_bro->off);
    pg->parent = par;
    nu_bro->parent = par;
    /*for (i=sp+1; i<pg->n_el; ++i) {
        nu_bro->recs[i-sp-1].key = pg->recs[i].key;
        nu_bro->recs[i-sp-1].val = pg->recs[i].off;
        nu_bro->recs[i-sp-1].off = pg->recs[i].off;
    }
    nu_bro->recs[i-sp-1].off = pg->recs[i].off;
    nu_bro->n_el = i-sp-1;*/
    memcpy(nu_bro->recs, pg->recs+sp+1, (pg->n_el-(sp+1))*sizeof(*pg->recs));
    nu_bro->n_el = pg->n_el-(sp+1);
    pg->n_el = sp;
    return par;
}

/** \todo missing unittest */
ret_t mmp_bt_insert(t_mmp_bt_s *bt, t_mmp_btkey key, t_mmp_btval val)
{
    t_pg_s *pg;
    t_mmp_btkey inkey;
    t_mmp_btoff inoff;
    int i;
    pg = bt->root;
reloop:
    if (pg->n_el>=bt->R)
        pg = pg_split(bt, pg);
    check_mapped(1, pg);
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
    return MMP_TAP_FAILED;
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

