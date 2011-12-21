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

/** \test test_mmp_list */
t_mmp_list_s *mmp_list_create(void)
{
    t_mmp_list_s * __restrict ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->nelems = 0;
    ret->head = ret->tail = NULL;
    return ret;
}

/** \test test_mmp_list */
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

/** \test test_mmp_list */
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

/** \test test_mmp_list */
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

/** \test test_mmp_list */
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

/** \test test_mmp_list */
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

/** \test test_mmp_list */
void* mmp_list_del_elem_by_data(t_mmp_list_s * __restrict list,
                                                            const void * data)
{
    t_mmp_listelem_s *p;
    if ((p = mmp_list_find_data(list, data))==NULL) return NULL;
    return mmp_list_del_elem(list, &p);
}

/** \test test_mmp_list */
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

