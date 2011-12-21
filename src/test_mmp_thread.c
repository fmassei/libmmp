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
#include "mmp_thread.h"

int glob;
static void* thtest(void* ptr)
{
    ++glob;
    mmp_thread_exit(0);
    ptr = ptr;
    return NULL;
}
static int test_threads(void)
{
    int i;
    t_mmp_thread id[10];
    glob = 0;
    for (i=0; i<10; ++i) {
        if (mmp_thread_create(thtest, NULL, &(id[i]))!=MMP_ERR_OK)
            return -1;
    }
    for (i=0; i<10; ++i)
        if (mmp_thread_join(&(id[i]))!=MMP_ERR_OK)
            return -1;
    if (glob!=10)
        return -1;
    return 0;
}

int main(void)
{
    if (    test_threads()!=0 )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

