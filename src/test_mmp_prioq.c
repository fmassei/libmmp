/*
    Copyright 2011 Francesco Massei

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
#include "mmp_prioq.h"

int main (void)
{
    t_mmp_prioq_s *prioq;
    char *a = "ABCDEFGHIJK";
    int p[] = { 6,10,3,1,5,1,7,8,4,2,-2 };
    int i;
    char *b;
    if ((prioq = mmp_prioq_create(5))==NULL)
        return EXIT_FAILURE;
    for (i=0; i<10; ++i) {
        if (mmp_prioq_insert(prioq, &a[i], p[i])!=MMP_ERR_OK)
            return EXIT_FAILURE;
    }
    if (mmp_prioq_pop(prioq, (void**)&b, &i)!=MMP_ERR_OK || *b!='B')
        return EXIT_FAILURE;
    if (mmp_prioq_pop(prioq, (void**)&b, &i)!=MMP_ERR_OK || *b!='H')
        return EXIT_FAILURE;
    if (mmp_prioq_pop(prioq, (void**)&b, &i)!=MMP_ERR_OK || *b!='G')
        return EXIT_FAILURE;
    mmp_prioq_destroy(&prioq);
    if (prioq!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

