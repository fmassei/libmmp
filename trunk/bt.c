typedef int t_off;
typedef int t_key;
typedef int t_val;

#define INVALID_OFF ((t_off)1)

typedef struct rec_s {
    t_off off;
    t_key key;
    t_val val;
} t_rec_s;

typedef struct pg_s {
    t_rec_s *recs;          /* ptr to secondary storage after mmap() */
    int n_el;               /* number of present elements */
    struct t_pg_s *parent;  /* ptr to parent page */
    t_off off;              /* file offset */
} t_pg_s;

typedef struct bt_s {
    int R;                  /* btree size (number of keys per page) */
    int pgsize;             /* page size */
    t_pg_s *root;           /* root page ptr */
    t_pg_s *pgoff;          /* array of present pages */
    int n_pgoff;            /* number of present pages */
} t_bt_s;

t_pg_s *create_empty_pg(void)
{
    t_pg_s *ret;
    ret = xmalloc(sizeof(*ret));
    ret->base = NULL;
    ret->n_el = 0;
    ret->parent = NULL;
    ret->off = INVALID_OFF;
    return ret;
}

t_bt_s *bt_create(int pgsize)
{
    t_bt_s *ret;
    ret = xmalloc(sizeof(*ret));
    ret->pg_size = pgsize;
    ret->R = get_R_from_pgsize(pgsize);
    ret->root = ret->pgoff = NULL;
    ret->n_pgoff = 0;
}

static int off_to_idx(t_bt_s *bt, t_off off)
{
    return off/bt->pgsize;
}

static int is_off_valid(t_off off)
{
    return (off!=INVALID_OFF);
}

static t_pg_s *page_from_off(t_bt_s *bt, t_off off)
{
    if (!is_off_valid)
        return NULL;
    return bt->pgoff[off_to_idx(bt, off)];
}

static int get_R_from_pgsize(int pgsize)
{
    int ret;
    ret = (pgsize / sizeof(t_rec_s)) - 1;
    return ret;
}

static ret_t check_mapped(int n_mp, ...)
{
    va_list va;
    va_start(va, n_mp);
    /* TODO */
    va_end(va);
}

ret_t bt_search(t_bt_s *bt, t_key key, t_val *val)
{
    t_pg_s *pg;
    t_key inkey;
    t_off inoff;
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
            pg = page_from_off(pg->recs[i].off);
            goto reloop;
        } else {
            memcpy(val, &pg->recs[i].val, sizeof(val));
            return MMP_ERR_OK;
        }
    }
    pg = page_from_off(pg->recs[i].off);
    goto reloop;
}

static ret_t pg_insert_in_pos(t_bt_s *bt, t_pg_s *pg, t_key key, t_val val,
                                            t_off off1, t_off off2, int pos)
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

static ret_t pg_insert(t_bt_s *bt, t_pg_s *pg, t_key key, t_val val,
                        t_off off1, t_off off2)
{
    int i, j;
    t_key inkey;
    for (i=0; i<pg->n_el; ++i) {
        inkey = pg->recs[i].key;
        if (inkey<key) {
            continue;
        } else if (inkey==key) {
            return MMP_ERR_DUPLICATE;
        } else {
            break; /* goto copy with right i value */
        }
    }
    return pg_insert_in_pos(bt, pg, key, val, off1, off2, i);
}

static t_pg_s *pg_split(t_bt_s *bt, t_pg_s *pg)
{
    int sp, i;
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
    pg_insert(bt, par, pg->recs[sp].key, pg->recs[sp].val, pg->off,nu_bro->off);
    pg->parent = par;
    nu_bro->parent = par;
    /*for (i=sp+1; i<pg->n_el; ++i) {
        nu_bro->recs[i-sp-1].key = pg->recs[i].key;
        nu_bro->recs[i-sp-1].val = pg->recs[i].off;
        nu_bro->recs[i-sp-1].off = pg->recs[i].off;
    }
    nu_bro->recs[i-sp-1].off = pg->recs[i].off;*/
    memcpy(nu_bro->recs, pg->recs+sp+1, (pg->n_el-(sp+1))*sizeof(*pg->recs));
    nu_bro->n_el = i-sp-1;
    pg->n_el = sp;
    return par;
}

ret_t bt_insert(t_bt_s *bt, t_key key, t_val val)
{
    t_pg_s *pg;
    t_key inkey;
    t_off inoff;
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
            return MMP_ERR_DUP;
        } else {
            inoff = pg->recs[i].off;
            if (is_off_valid(inoff)) {
                pg = page_from_off(inoff);
                goto reloop;
            } else {
                /* leaf insert (inoff is INVALID_OFF) */
                return pg_insert_in_pos(bt, pg, key, val, inoff, inoff, i); 
            }
        }
    }
    inoff = pg->recs[i].off;
    if (is_off_valid(inoff)) {
        pg = page_from_off(inoff);
        goto reloop;
    } else {
        /* leaf insert (inoff is INVALID_OFF) */
        return pg_insert_in_pos(bt, pg, key, val, inoff, inoff, i); 
    }
    return MMP_ERR_GENERIC;
}

