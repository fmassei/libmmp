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
#include "mmp_memory.h"

#ifndef NO_MMP_ALLOCATOR

/** \test mmp_allocator_unittest */
void *xmalloc(size_t size)
{
    return malloc(size);
}

/** \test mmp_allocator_unittest */
void *xcalloc(size_t count, size_t size)
{
    return calloc(count, size);
}

/** \test mmp_allocator_unittest */
void xfree(void *ptr)
{
    free(ptr);
}

/** \test mmp_allocator_unittest */
void *xrealloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_allocators(void)
{
    int *i_p, i;
    if ((i_p = xmalloc(sizeof(*i_p)))==NULL)
        return MMP_TAP_FAILED;
    xfree(i_p);
    if ((i_p = xcalloc(10, sizeof(*i_p)))==NULL)
        return MMP_TAP_FAILED;
    for (i=0; i<10; ++i)
        i_p[i] = i;
    if ((i_p = xrealloc(i_p, 20*sizeof(*i_p)))==NULL)
        return MMP_TAP_FAILED;
    for (i=10; i<20; ++i)
        i_p[i] = i;
    for (i=0; i<20; ++i)
        if (i_p[i]!=i)
            return MMP_TAP_FAILED;
    xfree(i_p);
    i_p = NULL;
    xfree(i_p); /* free NULL pointer, must result in no-op */
    return MMP_TAP_PASSED;
}

/* do the tests */
ret_t mmp_allocator_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp allocator functions", NULL,
                                        test_allocators()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

#endif /* MMP_ALLOCATOR */
