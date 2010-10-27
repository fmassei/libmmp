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
#include "mmp_funclist.h"

/** \test mmp_funclist_unittest */
t_mmp_funclist_s *mmp_funclist_create(void)
{
    t_mmp_funclist_s * __restrict ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->nelems = 0;
    ret->head = ret->tail = NULL;
    return ret;
}

/** \test mmp_funclist_unittest */
void mmp_funclist_destroy(t_mmp_funclist_s **list)
{
    t_mmp_funclistelem_s *p, *q;
    if (list==NULL || *list==NULL)
        return;
    for (p=(*list)->head; p!=NULL; p=q) {
        q = p->next;
        xfree(p);
    }
    MMP_XFREE_AND_NULL(*list);
}

/** \test mmp_funclist_unittest */
ret_t mmp_funclist_add_func(t_mmp_funclist_s * __restrict list, t_mmp_fp data)
{
    t_mmp_funclistelem_s *nu;
    MMP_CHECK_OR_RETURN((list!=NULL), MMP_ERR_PARAMS);
    MMP_XMALLOC_OR_RETURN(nu, MMP_ERR_ENOMEM);
    nu->data = data;
    nu->prev = NULL;
    nu->next = list->head;
    if (list->head)
        list->head->prev = nu;
    list->head = nu;
    if (list->tail==NULL)
        list->tail = nu;
    ++list->nelems;
    return MMP_ERR_OK;
}

/** \test mmp_funclist_unittest */
void *mmp_funclist_del_elem(t_mmp_funclist_s * __restrict list,
                                                t_mmp_funclistelem_s **elem)
{
    MMP_CHECK_OR_RETURN((list!=NULL && elem!=NULL && *elem!=NULL), NULL);
    if ((*elem)->prev!=NULL)
        (*elem)->prev->next = (*elem)->next;
    if ((*elem)->next!=NULL)
        (*elem)->next->prev = (*elem)->prev;
    if (list->head==*elem)
        list->head = (*elem)->next;
    if (list->tail==*elem)
        list->tail = (*elem)->prev;
    MMP_XFREE_AND_NULL(*elem);
    --list->nelems;
    return NULL;
}

/** \test mmp_funclist_unittest */
void* mmp_funclist_del_elem_by_func(t_mmp_funclist_s * __restrict list,
                                                        const t_mmp_fp data)
{
    t_mmp_funclistelem_s *p;
    if ((p = mmp_funclist_find_func(list, data))==NULL) return NULL;
    return mmp_funclist_del_elem(list, &p);
}

/** \test mmp_funclist_unittest */
t_mmp_funclistelem_s *mmp_funclist_find_func(
                                    const t_mmp_funclist_s * __restrict list,
                                    const t_mmp_fp data)
{
    t_mmp_funclistelem_s *p;
    if (list==NULL || data==NULL) return NULL;
    for(p=list->head; p!=NULL; p=p->next)
        if (p->data==data)
            return p;
    return NULL;
}

/** \todo missing unittest */
void mmp_funclist_lambda_elem(t_mmp_funclist_s * __restrict list,
                                            void(*fnc)(t_mmp_funclistelem_s *))
{
    t_mmp_funclistelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p);
}

/** \todo missing unittest */
void mmp_funclist_lambda_func(t_mmp_funclist_s * __restrict list,
                                            void(*fnc)(t_mmp_fp))
{
    t_mmp_funclistelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p->data);
}

/** \todo missing unittest */
void mmp_funclist_lambda_func_ext(t_mmp_funclist_s * __restrict list, 
                                                void(*fnc)(t_mmp_fp, void*),
                                                void *params)
{
    t_mmp_funclistelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p->data, params);
}

/** \todo missing unittest */
void mmp_funclist_lambda_func_ext_rev(t_mmp_funclist_s * __restrict list,
                                                void(*fnc)(t_mmp_fp, void*),
                                                void *params)
{
    t_mmp_funclistelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->tail; p!=NULL; p=p->prev)
        fnc(p->data, params);
}

/** \todo missing unittest */
void mmp_funclist_swap_elems(t_mmp_funclistelem_s *e1,
                             t_mmp_funclistelem_s *e2)
{
    t_mmp_fp dt;
    if (e1==e2) return;
    dt = e1->data;
    e1->data = e2->data;
    e2->data = dt;
}

#ifdef UNIT_TESTING
static int f1(int a) { return a+1; }
static int f2(int a) { return a+2; }
typedef int(*my_fp)(int);

/* create an example list and fill it with some functions */
static t_mmp_funclist_s *create_dummy_filled_list(void)
{
    t_mmp_funclist_s *ret;
    ret_t err;
    if ((ret = mmp_funclist_create())==NULL)
        return NULL;
    if (    ((err = mmp_funclist_add_func(ret, (t_mmp_fp)f1))!=MMP_ERR_OK) ||
            ((err = mmp_funclist_add_func(ret, (t_mmp_fp)f2))!=MMP_ERR_OK)  ) {
        mmp_setError(err);
        goto badexit;
    }
    return ret;
badexit:
    mmp_funclist_destroy(&ret);
    return ret;
}
/* create and delete an empty list */
static t_mmp_tap_result_e test_create_delete(void)
{
    t_mmp_funclist_s *list;
    if ((list = mmp_funclist_create())==NULL)
        return MMP_TAP_FAILED;
    mmp_funclist_destroy(&list);
    if (list!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
/* create and delete a filled list of 10 ints, checking them afterwards */
static t_mmp_tap_result_e test_add_check(void)
{
    t_mmp_funclist_s *list;
    t_mmp_funclistelem_s *p;
    int i = 1;
    my_fp fnc;
    if ((list = create_dummy_filled_list())==NULL)
        return MMP_TAP_FAILED;
    for (p=list->head; p!=NULL; p=p->next) {
        fnc = (my_fp)p->data;
        i = fnc(i);
    }
    if (i!=4)
        return MMP_TAP_FAILED;
    mmp_funclist_destroy(&list);
    return MMP_TAP_PASSED;
}
/* do the tests */
ret_t mmp_funclist_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "funclist create and delete", NULL,
                                        test_create_delete()))!=MMP_ERR_OK) ||
            ((ret=mmp_tap_test(cycle, "funclist add and check", NULL,
                                        test_add_check()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

