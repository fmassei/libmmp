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

/** \test test_mmp_memory */
void *xmalloc(size_t size)
{
    return malloc(size);
}

/** \test test_mmp_memory */
void *xcalloc(size_t count, size_t size)
{
    return calloc(count, size);
}

/** \test test_mmp_memory */
void xfree(void *ptr)
{
    free(ptr);
}

/** \test test_mmp_memory */
void *xrealloc(void *ptr, size_t size)
{
    return realloc(ptr, size);
}

#endif /* MMP_ALLOCATOR */
