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
#include "mmp_system.h"
#include "mmp_btree.h"

static int test_bt(void)
{
    t_mmp_bt_s *bt;
    int i, r, rv;
    remove("test.bt");
    if ((bt = mmp_bt_create("test.bt", mmp_system_getPageAlignment()))==NULL)
        return -1;
    for (i=0; i<100000; ++i) {
        if (mmp_bt_insert(bt, i, (i+1))!=MMP_ERR_OK) {
            mmp_trace_print(stdout);
            return -1;
        }
    }
    for (i=0; i<100; ++i) {
        r = (int)(100000*(rand()/(RAND_MAX+1.0)));
        if (mmp_bt_search(bt, r, &rv)!=MMP_ERR_OK) return -1;
        if (rv!=r+1) return -1;
    }
    mmp_bt_destroy(&bt);
    if (bt!=NULL)
        return -1;
    remove("test.bt");
    return 0;
}

int main(void)
{
    if (    test_bt()!=0    )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

