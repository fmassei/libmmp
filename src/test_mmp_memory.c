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
#include "mmp_error.h"
#include "mmp_memory.h"

static int test_allocators(void)
{
    int *i_p, i;
    if ((i_p = xmalloc(sizeof(*i_p)))==NULL)
        return -1;
    xfree(i_p);
    if ((i_p = xcalloc(10, sizeof(*i_p)))==NULL)
        return -1;
    for (i=0; i<10; ++i)
        i_p[i] = i;
    if ((i_p = xrealloc(i_p, 20*sizeof(*i_p)))==NULL)
        return -1;
    for (i=10; i<20; ++i)
        i_p[i] = i;
    for (i=0; i<20; ++i)
        if (i_p[i]!=i)
            return -1;
    xfree(i_p);
    i_p = NULL;
    xfree(i_p); /* free NULL pointer, must result in no-op */
    return 0;
}

/* do the tests */
int main(void)
{
    if (    test_allocators()!=0    )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

