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
#include "mmp_htab.h"

/** \test mmp_htab_unittest */
t_mmp_htab_s *mmp_htab_create(size_t size)
{
    t_mmp_htab_s * __restrict ret;
    size_t i;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->size = size;
    if ((ret->ptrs = xmalloc(sizeof(*ret->ptrs)*size))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(ret);
        return NULL;
    }
    for (i=0; i<size; ++i)
        ret->ptrs[i] = NULL;
    return ret;
}

/** \test mmp_htab_unittest */
void mmp_htab_destroy(t_mmp_htab_s **ptr)
{
    t_mmp_htab_elem_s *p, *q;
    unsigned int i;
    if (ptr==NULL || *ptr==NULL) return;
    for (i=0; i<(*ptr)->size; ++i) {
        if ((*ptr)->ptrs[i]==NULL)
            continue;
        for (p=(*ptr)->ptrs[i]; p!=NULL; p=q) {
            q = p->next;
            xfree(p);
        }
    }
    MMP_XFREE_AND_NULL(*ptr);
}

/** \test mmp_htab_unittest */
void mmp_htab_destroy_with_data(t_mmp_htab_s **ptr, void(*datadel)(void**))
{
    t_mmp_htab_elem_s *p, *q;
    unsigned int i;
    if (ptr==NULL || *ptr==NULL) return;
    for (i=0; i<(*ptr)->size; ++i) {
        if ((*ptr)->ptrs[i]==NULL)
            continue;
        for (p=(*ptr)->ptrs[i]; p!=NULL; p=q) {
            q = p->next;
            if (p->key!=NULL)
                xfree(p->key);
            if (p->val!=NULL && datadel!=NULL)
                datadel(&(p->val));
            xfree(p);
        }
    }
    xfree((*ptr)->ptrs);
    MMP_XFREE_AND_NULL(*ptr);
}

/** \test mmp_htab_unittest */
unsigned int mmp_htab_hash(const t_mmp_htab_s * __restrict htab,
                                        const char * __restrict str, int max)
{
    unsigned int hashval;
    int i;
    for (hashval=0, i=0; *str!='\0' && i<max; ++str, ++i)
        hashval = *str+31*hashval;
    return hashval % htab->size;
}

/** \test mmp_htab_unittest */
static t_mmp_htab_elem_s *lookup(const t_mmp_htab_s * __restrict htab, 
                                    const char * __restrict key, int keylen)
{
    t_mmp_htab_elem_s *np;
    MMP_CHECK_OR_RETURN((htab!=NULL && key!=NULL), NULL);
    for (np=htab->ptrs[mmp_htab_hash(htab, key, keylen)];np!=NULL;np=np->next)
        if (!strcmp(key, np->key))
            return np;
    return NULL;
}

/** \test mmp_htab_unittest */
void *mmp_htab_lookup(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key)
{
    t_mmp_htab_elem_s *np;
    MMP_CHECK_OR_RETURN((htab!=NULL && key!=NULL), NULL);
    if ((np = lookup(htab, key, UINT_MAX))!=NULL)
        return np->val;
    return NULL;
}

/** \test mmp_htab_unittest */
void *mmp_htab_lookup_nz(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, int keylen)
{
    t_mmp_htab_elem_s *np;
    MMP_CHECK_OR_RETURN((htab!=NULL && key!=NULL), NULL);
    if ((np = lookup(htab, key, keylen))!=NULL)
        return np->val;
    return NULL;
}

/** \test mmp_htab_unittest */
ret_t mmp_htab_install(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, void *val)
{
    t_mmp_htab_elem_s *np;
    unsigned int hashval;
    MMP_CHECK_OR_RETURN((htab!=NULL && key!=NULL), MMP_ERR_PARAMS);
    if ((np = lookup(htab, key, UINT_MAX))==NULL) {
        MMP_XMALLOC_OR_RETURN(np, MMP_ERR_ENOMEM);
        hashval = mmp_htab_hash(htab, key, UINT_MAX);
        np->key = xstrdup(key);
        np->val = val;
        np->next = htab->ptrs[hashval];
        np->prev = NULL;
        if (htab->ptrs[hashval]!=NULL)
            htab->ptrs[hashval]->prev = np;
        htab->ptrs[hashval] = np;
    } else {
        np->val = val;
    }
    return MMP_ERR_OK;
}

/** \test mmp_htab_unittest */
ret_t mmp_htab_delete(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key)
{
    t_mmp_htab_elem_s *np;
    unsigned int hashval;
    MMP_CHECK_OR_RETURN((htab!=NULL && key!=NULL), MMP_ERR_PARAMS);
    if ((np = lookup(htab, key, UINT_MAX))!=NULL) {
        hashval = mmp_htab_hash(htab, key, UINT_MAX);
        if (np->next!=NULL) np->next->prev = np->prev;
        if (np->prev!=NULL) np->prev->next = np->next;
        if (htab->ptrs[hashval]==np)
            htab->ptrs[hashval] = np->prev;
        xfree(np);
    }
    return MMP_ERR_OK;
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_create_destroy(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return MMP_TAP_FAILED;
    mmp_htab_destroy(&htab);
    if (htab!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

static t_mmp_tap_result_e test_insert_delete(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return MMP_TAP_FAILED;
    if (
            (mmp_htab_install(htab, "key1", "value1")!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", "value2")!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", "value3")!=MMP_ERR_OK)   )
        return MMP_TAP_FAILED;
    if (strcmp("value3", (char*)mmp_htab_lookup(htab, "key2")))
        return MMP_TAP_FAILED;
    if (mmp_htab_delete(htab, "key2")!=MMP_ERR_OK)
        return MMP_TAP_FAILED;
    if (mmp_htab_lookup(htab, "key2")!=NULL)
        return MMP_TAP_FAILED;
    mmp_htab_destroy(&htab);
    if (htab!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

static void free_test_data_v(void **ptr)
{
    MMP_XFREE_AND_NULL(*ptr);
}
static t_mmp_tap_result_e test_insert_delete_2(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return MMP_TAP_FAILED;
    if (
            (mmp_htab_install(htab, "key1", xstrdup("value1"))!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", xstrdup("value2"))!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", xstrdup("value3"))!=MMP_ERR_OK)   )
        return MMP_TAP_FAILED;
    if (strcmp("value3", (char*)mmp_htab_lookup(htab, "key2")))
        return MMP_TAP_FAILED;
    if (mmp_htab_lookup_nz(htab, "key2", 3)!=NULL)
        return MMP_TAP_FAILED;
    mmp_htab_destroy_with_data(&htab, free_test_data_v);
    if (htab!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

/* do the tests */
ret_t mmp_htab_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret = mmp_tap_test(cycle, "htab create and delete", NULL,
                                 test_create_destroy()))!=MMP_ERR_OK) ||
            ((ret = mmp_tap_test(cycle, "htab insert and delete", NULL,
                                 test_insert_delete()))!=MMP_ERR_OK) ||
            ((ret = mmp_tap_test(cycle, "htab insert and delete 2", NULL,
                                 test_insert_delete_2()))!=MMP_ERR_OK)
        )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

