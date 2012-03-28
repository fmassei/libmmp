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
    t_mmp_i_vect_s *vect;
    int i, k=0;
    if ((vect = mmp_i_vect_create(10,1))==NULL)
        return EXIT_FAILURE;
    for (i=0; i<5; ++i) {
        if (mmp_i_vect_push_back(vect, i+1)!=MMP_ERR_OK)
            return EXIT_FAILURE;
    }
    do {
        if (vect->n_data>0)
            k += vect->data[vect->n_data-1];
    } while (mmp_i_vect_pop_back(vect)==MMP_ERR_OK);
    if (k!=10+1+2+3+4+5)
        return EXIT_FAILURE;
    mmp_i_vect_destroy(&vect);
    if (vect!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

