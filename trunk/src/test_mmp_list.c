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
#include <stdio.h>
#include <stdlib.h>
#include "mmp_list.h"

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
static int test_create_delete(void)
{
    t_mmp_list_s *list;
    if ((list = mmp_list_create())==NULL)
        return -1;
    mmp_list_delete(&list);
    if (list!=NULL)
        return -1;
    return 0;
}
/* create and delete a filled list of 10 ints, checking them afterwards */
static int test_add_check(void)
{
    t_mmp_list_s *list;
    t_mmp_listelem_s *p;
    int ret = 0;
    int i, *int_data;
    ret = 0;
    if ((list = create_dummy_filled_list(10))==NULL)
        return -1;
    for (i=9, p=list->head; p!=NULL; --i, p=p->next) {
        int_data = (int*)p->data;
        if (*int_data!=i)
            ret = -1;
    }
    mmp_list_delete_withdata(&list, gen_deleter);
    return ret;
}
/* creates and delete a list, checking during the operation */
static int test_delete(void)
{
    t_mmp_list_s *list;
    int i1=42, i2=43;
    if ((list = create_dummy_filled_list(10))==NULL)
        return -1;
    if (mmp_list_add_data(list, &i1)!=MMP_ERR_OK)
        return -1;
    if (mmp_list_add_data(list, &i2)!=MMP_ERR_OK)
        return -1;
    if (list->nelems!=12)
        return -1;
    mmp_list_del_elem_by_data(list, &i2);
    if (mmp_list_find_data(list, &i2)!=NULL ||
            mmp_list_find_data(list, &i1)==NULL)
        return -1;
    if (list->nelems!=11)
        return -1;
    mmp_list_del_elem_by_data(list, &i1);
    if (list->nelems!=10)
        return -1;
    return 0;
}
/* test sorted lists */
static int sortcomp(const void *v1, const void *v2)
{
    if (v1>v2) return -1;
    if (v1<v2) return 1;
    return 0;
}
static int test_sorted(void)
{
    t_mmp_list_s *list;
    t_mmp_listelem_s *p;
    int i, v[] = { 23, 12, 6, 22, 54 }, t[] = { 6, 12, 22, 23, 54 };
    if ((list = mmp_list_create())==NULL)
        return -1;
    for (i=0; i<sizeof(v)/sizeof(v[0]); ++i)
        if (mmp_list_add_data_sorted(list, (void*)v[i], sortcomp)!=MMP_ERR_OK)
            return -1;
    for (p=list->head, i=0; p!=NULL; p=p->next, ++i)
        if (((int)p->data)!=t[i])
            return -1;
    mmp_list_del_elem_by_data(list, (void*)t[2]);
    mmp_list_del_elem_by_data(list, (void*)t[4]);
    mmp_list_del_elem_by_data(list, (void*)t[3]);
    mmp_list_del_elem_by_data(list, (void*)t[0]);
    if (((int)list->head->data)!=t[1])
        return -1;
    mmp_list_delete(&list);
    return 0;
}
/* do the tests */
int main(void)
{
    ret_t ret;
    if (    test_create_delete()!=0 ||
            test_delete()!=0 ||
            test_sorted()!=0 ||
            test_add_check()!=0 )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

