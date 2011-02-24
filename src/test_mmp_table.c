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
#include "mmp_table.h"

int main(void)
{
    t_mmp_table_s *table;
    int i = 0, *dt;
    if ((table = mmp_table_create(10))==NULL)
        return EXIT_FAILURE;
    for (i=0; i<10; ++i) {
        if ((dt = xmalloc(sizeof(*dt)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return EXIT_FAILURE;
        }
        *dt = i;
        if (mmp_table_add(table, dt)!=i)
            return EXIT_FAILURE;
    }
    for (i=0; i<10; ++i) {
        dt = mmp_table_del(table, 0);
        if (*dt!=i)
            return EXIT_FAILURE;
        xfree(dt);
    }
    mmp_table_destroy(&table);
    if (table!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

