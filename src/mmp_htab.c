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

