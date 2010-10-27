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
#include "mmp_list.h"

/** \test mmp_list_unittest */
t_mmp_list_s *mmp_list_create(void)
{
    t_mmp_list_s * __restrict ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->nelems = 0;
    ret->head = ret->tail = NULL;
    return ret;
}

/** \test mmp_list_unittest */
void mmp_list_delete(t_mmp_list_s **list)
{
    t_mmp_listelem_s *p, *q;
    if (list==NULL || *list==NULL)
        return;
    for (p=(*list)->head; p!=NULL; p=q) {
        q = p->next;
        xfree(p);
    }
    MMP_XFREE_AND_NULL(*list);
}

/** \test mmp_list_unittest */
void mmp_list_delete_withdata(t_mmp_list_s **list, void(*cback)(void**))
{
    t_mmp_listelem_s *p, *q;
    if (list==NULL || *list==NULL)
        return;
    for (p=(*list)->head; p!=NULL; p=q) {
        q = p->next;
        cback(&p->data);
        xfree(p);
    }
    MMP_XFREE_AND_NULL(*list);
}

/** \test mmp_list_unittest */
ret_t mmp_list_add_data(t_mmp_list_s * __restrict list, void * data)
{
    t_mmp_listelem_s *nu;
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

/** \test mmp_list_unittest */
ret_t mmp_list_add_data_sorted(t_mmp_list_s * __restrict list, void *data,
                                                        t_mmp_comparer_f comp)
{
    t_mmp_listelem_s *nu, *p;
    MMP_CHECK_OR_RETURN((list!=NULL), MMP_ERR_PARAMS);
    if (list->head==NULL)
        return mmp_list_add_data(list, data);
    MMP_XMALLOC_OR_RETURN(nu, MMP_ERR_ENOMEM);
    nu->data = data;
    for (p=list->head; p!=NULL; p=p->next) {
        if (comp(p->data, nu->data)<=0)
            break;
    }
    if (p==NULL) {
        nu->prev = list->tail;
        nu->next = NULL;
        list->tail->next = nu;
        list->tail = nu;
    } else if (p==list->head) {
        nu->prev = NULL;
        nu->next = list->head;
        list->head->prev = nu;
        list->head = nu;
    } else {
        nu->prev = p->prev;
        if (p->prev)
            p->prev->next = nu;
        p->prev = nu;
        nu->next = p;
    }
    ++list->nelems;
    return MMP_ERR_OK;
}

/** \test mmp_list_unittest */
void *mmp_list_del_elem(t_mmp_list_s * __restrict list,
                                                    t_mmp_listelem_s **elem)
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

/** \test mmp_list_unittest */
void* mmp_list_del_elem_by_data(t_mmp_list_s * __restrict list,
                                                            const void * data)
{
    t_mmp_listelem_s *p;
    if ((p = mmp_list_find_data(list, data))==NULL) return NULL;
    return mmp_list_del_elem(list, &p);
}

/** \test mmp_list_unittest */
t_mmp_listelem_s *mmp_list_find_data(const t_mmp_list_s * __restrict list,
                                                            const void * data)
{
    t_mmp_listelem_s *p;
    if (list==NULL || data==NULL) return NULL;
    for(p=list->head; p!=NULL; p=p->next)
        if (p->data==data)
            return p;
    return NULL;
}

/** \todo missing unittest */
t_mmp_listelem_s *mmp_list_find_data_lambda(
                                        const t_mmp_list_s * __restrict list,
                                        const void * data,
                                        t_mmp_comparer_f comp)
{
    t_mmp_listelem_s *p;
    if (list==NULL || data==NULL) return NULL;
    for (p=list->head; p!=NULL; p=p->next)
        if (comp(data, p->data)==0)
            return p;
    return NULL;
}

/** \todo missing unittest */
t_mmp_listelem_s *mmp_list_find_data_sorted(
                                        const t_mmp_list_s * __restrict list,
                                        const void * data,
                                        t_mmp_comparer_f comp)
{
    t_mmp_listelem_s *p;
    if (list==NULL || data==NULL) return NULL;
    for (p=list->head; p!=NULL; p=p->next) {
        if (comp(data, p->data)<0) continue;
        else if (comp(data, p->data)==0) return p;
        else return NULL;
    }
    return NULL;
}

/** \todo missing unittest */
void mmp_list_lambda_elem(t_mmp_list_s * __restrict list,
                                                void(*fnc)(t_mmp_listelem_s *))
{
    t_mmp_listelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p);
}

/** \todo missing unittest */
void mmp_list_lambda_data(t_mmp_list_s * __restrict list, void(*fnc)(void*))
{
    t_mmp_listelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p->data);
}

/** \todo missing unittest */
void mmp_list_lambda_data_ext(t_mmp_list_s * __restrict list, 
                                                void(*fnc)(void*, void*),
                                                void *params)
{
    t_mmp_listelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->head; p!=NULL; p=p->next)
        fnc(p->data, params);
}

/** \todo missing unittest */
void mmp_list_lambda_data_ext_rev(t_mmp_list_s * __restrict list,
                                                void(*fnc)(void*, void*),
                                                void *params)
{
    t_mmp_listelem_s *p;
    if (list==NULL || fnc==NULL) return;
    for (p=list->tail; p!=NULL; p=p->prev)
        fnc(p->data, params);
}

/** \todo missing unittest */
void mmp_list_swap_elems(t_mmp_listelem_s *e1, t_mmp_listelem_s *e2)
{
    void *dt;
    if (e1==e2) return;
    dt = e1->data;
    e1->data = e2->data;
    e2->data = dt;
}

static t_mmp_listelem_s *mmp_list_find_min(t_mmp_listelem_s *start,
                                                    t_mmp_comparer_f comp)
{
    t_mmp_listelem_s *min;
    for(min=start; start!=NULL; start=start->next) {
        if (comp(min->data, start->data)>0)
            min = start;
    }
    return min;
}

/** \todo missing unittest */
void mmp_list_sort_by_data(t_mmp_list_s * __restrict list,
                                                    t_mmp_comparer_f comp)
{
    t_mmp_listelem_s *p, *n;
    for (p=list->head; p!=NULL; p=p->next) {
        n = mmp_list_find_min(p, comp);
        mmp_list_swap_elems(p, n);
    }
}

#ifdef UNIT_TESTING
/* generic deleter callback */
static void gen_deleter(void **data) {
    MMP_XFREE_AND_NULL(*data);
}
/* create an example list and fill it with some integers */
static t_mmp_list_s *create_dummy_filled_list(int num)
{
    t_mmp_list_s *ret;
    int *int_data, i;
    ret_t err;
    if ((ret = mmp_list_create())==NULL)
        return NULL;
    for (i=0; i<num; ++i) {
        if ((int_data = xmalloc(sizeof(*int_data)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            goto badexit;
        }
        *int_data = i;
        if ((err = mmp_list_add_data(ret, int_data))!=MMP_ERR_OK) {
            mmp_setError(err);
            goto badexit;
        }
    }
    return ret;
badexit:
    mmp_list_delete_withdata(&ret, gen_deleter);
    return ret;
}
/* create and delete an empty list */
static t_mmp_tap_result_e test_create_delete(void)
{
    t_mmp_list_s *list;
    if ((list = mmp_list_create())==NULL)
        return MMP_TAP_FAILED;
    mmp_list_delete(&list);
    if (list!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
/* create and delete a filled list of 10 ints, checking them afterwards */
static t_mmp_tap_result_e test_add_check(void)
{
    t_mmp_list_s *list;
    t_mmp_listelem_s *p;
    enum mmp_tap_result_e ret;
    int i, *int_data;
    ret = MMP_TAP_PASSED;
    if ((list = create_dummy_filled_list(10))==NULL)
        return MMP_TAP_FAILED;
    for (i=9, p=list->head; p!=NULL; --i, p=p->next) {
        int_data = (int*)p->data;
        if (*int_data!=i)
            ret = MMP_TAP_FAILED;
    }
    mmp_list_delete_withdata(&list, gen_deleter);
    return ret;
}
/* creates and delete a list, checking during the operation */
static t_mmp_tap_result_e test_delete(void)
{
    t_mmp_list_s *list;
    int i1=42, i2=43;
    if ((list = create_dummy_filled_list(10))==NULL)
        return MMP_TAP_FAILED;
    if (mmp_list_add_data(list, &i1)!=MMP_ERR_OK)
        return MMP_TAP_FAILED;
    if (mmp_list_add_data(list, &i2)!=MMP_ERR_OK)
        return MMP_TAP_FAILED;
    if (list->nelems!=12)
        return MMP_TAP_FAILED;
    mmp_list_del_elem_by_data(list, &i2);
    if (mmp_list_find_data(list, &i2)!=NULL ||
            mmp_list_find_data(list, &i1)==NULL)
        return MMP_TAP_FAILED;
    if (list->nelems!=11)
        return MMP_TAP_FAILED;
    mmp_list_del_elem_by_data(list, &i1);
    if (list->nelems!=10)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
/* test sorted lists */
static int sortcomp(const void *v1, const void *v2)
{
    if (v1>v2) return -1;
    if (v1<v2) return 1;
    return 0;
}
static t_mmp_tap_result_e test_sorted(void)
{
    t_mmp_list_s *list;
    t_mmp_listelem_s *p;
    int i, v[] = { 23, 12, 6, 22, 54 }, t[] = { 6, 12, 22, 23, 54 };
    if ((list = mmp_list_create())==NULL)
        return MMP_TAP_FAILED;
    for (i=0; i<sizeof(v)/sizeof(v[0]); ++i)
        if (mmp_list_add_data_sorted(list, (void*)v[i], sortcomp)!=MMP_ERR_OK)
            return MMP_TAP_FAILED;
    for (p=list->head, i=0; p!=NULL; p=p->next, ++i)
        if (((int)p->data)!=t[i])
            return MMP_TAP_FAILED;
    mmp_list_del_elem_by_data(list, (void*)t[2]);
    mmp_list_del_elem_by_data(list, (void*)t[4]);
    mmp_list_del_elem_by_data(list, (void*)t[3]);
    mmp_list_del_elem_by_data(list, (void*)t[0]);
    if (((int)list->head->data)!=t[1])
        return MMP_TAP_FAILED;
    mmp_list_delete(&list);
    return MMP_TAP_PASSED;
}
/* do the tests */
ret_t mmp_list_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "list_create and mmp_list_delete", NULL,
                                        test_create_delete()))!=MMP_ERR_OK) ||
            ((ret=mmp_tap_test(cycle, "list deletion", NULL,
                                        test_delete()))!=MMP_ERR_OK) ||
            ((ret=mmp_tap_test(cycle, "list sorted", NULL,
                                        test_sorted()))!=MMP_ERR_OK) ||
            ((ret=mmp_tap_test(cycle, "list add and check", NULL,
                                        test_add_check()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

