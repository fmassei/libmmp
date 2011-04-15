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
#include "mmp_htab.h"

static int test_create_destroy(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return -1;
    mmp_htab_destroy(&htab);
    if (htab!=NULL)
        return -1;
    return 0;
}

static int test_insert_delete(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return -1;
    if (
            (mmp_htab_install(htab, "key1", "value1")!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", "value2")!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", "value3")!=MMP_ERR_OK)   )
        return -1;
    if (strcmp("value3", (char*)mmp_htab_lookup(htab, "key2")))
        return -1;
    if (mmp_htab_delete(htab, "key2")!=MMP_ERR_OK)
        return -1;
    if (mmp_htab_lookup(htab, "key2")!=NULL)
        return -1;
    mmp_htab_destroy(&htab);
    if (htab!=NULL)
        return -1;
    return 0;
}

static void free_test_data_v(void **ptr)
{
    MMP_XFREE_AND_NULL(*ptr);
}
static int test_insert_delete_2(void)
{
    t_mmp_htab_s *htab;
    if ((htab = mmp_htab_create(101))==NULL)
        return -1;
    if (
            (mmp_htab_install(htab, "key1", xstrdup("value1"))!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", xstrdup("value2"))!=MMP_ERR_OK) ||
            (mmp_htab_install(htab, "key2", xstrdup("value3"))!=MMP_ERR_OK)   )
        return -1;
    if (strcmp("value3", (char*)mmp_htab_lookup(htab, "key2")))
        return -1;
    if (mmp_htab_lookup_nz(htab, "key2", 3)!=NULL)
        return -1;
    mmp_htab_destroy_with_data(&htab, free_test_data_v);
    if (htab!=NULL)
        return -1;
    return 0;
}

/* do the tests */
int main(void)
{
    if (    test_create_destroy()!=0 ||
            test_insert_delete()!=0 ||
            test_insert_delete_2()!=0  ) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

