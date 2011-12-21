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
#include "mmp_funclist.h"

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
static int test_create_delete(void)
{
    t_mmp_funclist_s *list;
    if ((list = mmp_funclist_create())==NULL)
        return -1;
    mmp_funclist_destroy(&list);
    if (list!=NULL)
        return -1;
    return 0;
}
/* create and delete a filled list of 10 ints, checking them afterwards */
static int test_add_check(void)
{
    t_mmp_funclist_s *list;
    t_mmp_funclistelem_s *p;
    int i = 1;
    my_fp fnc;
    if ((list = create_dummy_filled_list())==NULL)
        return -1;
    for (p=list->head; p!=NULL; p=p->next) {
        fnc = (my_fp)p->data;
        i = fnc(i);
    }
    if (i!=4)
        return -1;
    mmp_funclist_destroy(&list);
    return 0;
}

/* do the tests */
int main(void)
{
    if (    test_create_delete()!=0 ||
            test_add_check()!=0 )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

