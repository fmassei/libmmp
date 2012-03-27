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
#include "mmp_gen_vect.h"

int main(void)
{
    t_mmp_ivect_s *vect;
    int i, k;
    if ((vect = mmp_ivect_create(10))==NULL)
        return EXIT_FAILURE;
    for (i=0; i<10; ++i) {
        if (mmp_ivect_push(vect, i)!=i)
            return EXIT_FAILURE;
    }
    for (i=0; i<10; ++i) {
        k = mmp_ivect_del(vect, 0);
        if (k!=i)
            return EXIT_FAILURE;
    }
    mmp_ivect_destroy(&vect);
    if (vect!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

