/*
    Copyright 2011 Francesco Massei

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
#include "mmp_prioq.h"
#include <math.h>
#include "mmp_memory.h"
#include "mmp_trace.h"

/** internal prioq node */
typedef struct pnode_s {
    int prio;
    void *data;
} t_pnode_s;
/** internal prioq representation */
struct mmp_prioq_s {
    t_pnode_s **arr;
    size_t n, n_alloc;
};

/** \test test_prioq */
t_mmp_prioq_s *mmp_prioq_create(size_t prealloc)
{
    t_mmp_prioq_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->n = 0;
    ret->n_alloc = prealloc;
    ret->arr = NULL;
    if (prealloc>0) {
        if ((ret->arr = xmalloc(prealloc*sizeof(*(ret->arr))))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            xfree(ret);
            return NULL;
        }
    }
    return ret;
}

/** \test test_prioq */
void mmp_prioq_destroy(t_mmp_prioq_s **prioq)
{
    mmp_prioq_destroy_withdata(prioq, NULL);
}

/** \todo missing unittest */
void mmp_prioq_destroy_withdata(t_mmp_prioq_s **prioq, void(*cback)(void**))
{
    size_t i;
    if (prioq==NULL || *prioq==NULL) return;
    for (i=0; i<(*prioq)->n; ++i) {
        if (cback!=NULL)
            cback(&(*prioq)->arr[i]->data);
        xfree((*prioq)->arr[i]);
    }
    xfree((*prioq)->arr);
    MMP_XFREE_AND_NULL(*prioq);
}

/** internal prioq enlarge buffer */
static ret_t enlarge(t_mmp_prioq_s *prioq)
{
    t_pnode_s **nu_arr = NULL;
    size_t nu_alloc = prioq->n_alloc + 10;
    if ((nu_arr = xrealloc(prioq->arr, sizeof(*(prioq->arr))*nu_alloc))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    prioq->arr = nu_arr;
    prioq->n_alloc += 10;
    return MMP_ERR_OK;
}

/** internal prioq node creation */
static t_pnode_s *pnode_create(void *data, int prio)
{
    t_pnode_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->data = data;
    ret->prio = prio;
    return ret;
}

#define LCHILD(_N)  (2*(_N)+1)
#define RCHILD(_N)  (2*(_N)+2)
#define PARENT(_N)  (floor(((_N)-1)/2))
#define SWAP_PTR(_P1, _P2)  do { \
        void *tmp = (_P2); (_P2) = (_P1); (_P1) = tmp; \
    } while(0)

/** internal reposition the last inserted element */
static void sort_last(t_mmp_prioq_s *prioq)
{
    size_t n, par;
    if ((n = prioq->n-1)==0)
        return;
    do {
        par = PARENT(n);
        if (prioq->arr[n]->prio <= prioq->arr[par]->prio)
            break;
        SWAP_PTR(prioq->arr[n], prioq->arr[par]);
        n = par;
    } while(n!=0);
}

/** internal reposition the root note */
static void sort_root(t_mmp_prioq_s *prioq)
{
    size_t n, c1, c2, cb;
    n = 0;
    do {
        c1 = LCHILD(n);
        c2 = RCHILD(n);
        if (prioq->arr[n]->prio < prioq->arr[c1]->prio) {
            if (prioq->arr[n]->prio < prioq->arr[c2]->prio) {
                cb = (prioq->arr[c1]->prio > prioq->arr[c2]->prio) ? c1 : c2;
                SWAP_PTR(prioq->arr[n], prioq->arr[cb]);
                n = cb;
            } else {
                SWAP_PTR(prioq->arr[n], prioq->arr[c1]);
                n = c1;
            }
        } else {
            if (prioq->arr[n]->prio < prioq->arr[c2]->prio) {
                SWAP_PTR(prioq->arr[n], prioq->arr[c2]);
                n = c2;
            } else {
                break;
            }
        }
    } while(n<prioq->n);
}

/** \test test_prioq */
ret_t mmp_prioq_insert(t_mmp_prioq_s *prioq, void *data, int prio)
{
    t_pnode_s *nu_node;
    MMP_CHECK_OR_RETURN((prioq!=NULL), MMP_ERR_PARAMS);
    if (prioq->n>=prioq->n_alloc) {
        if (enlarge(prioq)!=MMP_ERR_OK) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_ERR_ENOMEM;
        }
    }
    if ((nu_node = pnode_create(data, prio))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return MMP_ERR_ENOMEM;
    }
    prioq->arr[prioq->n++] = nu_node;
    sort_last(prioq);
    return MMP_ERR_OK;
}

/** \test test_prioq */
ret_t mmp_prioq_pop(t_mmp_prioq_s *prioq, void **data, int *prio)
{
    MMP_CHECK_OR_RETURN((prioq!=NULL), MMP_ERR_PARAMS);
    if (prioq->n<=0)
        return MMP_ERR_EMPTY;
    if (data!=NULL) *data = prioq->arr[0]->data;
    if (prio!=NULL) *prio = prioq->arr[0]->prio;
    xfree(prioq->arr[0]);
    SWAP_PTR(prioq->arr[0], prioq->arr[--prioq->n]);
    sort_root(prioq);
    return MMP_ERR_OK;
}

