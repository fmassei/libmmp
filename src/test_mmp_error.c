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

static int test_getdesc(void)
{
    if (!strcmp(mmp_error_ret_getdesc(MMP_ERR_SHM),"Shared memory error"))
        return 0;
    return -1;
}

int main(void)
{
    if (    test_getdesc()!=0   )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

