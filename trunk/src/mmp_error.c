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
#include "mmp_error.h"

static const char * const error_strings[] = {
    "No error",
    "Generic error",
    "Wrong params",
    "Not enough memory",
    "File error",
    "Dynamic linking error",
    "Parsing error",
    "Semaphore error",
    "Shared memory error",
    "Socket error",
    "Syncronization error",
    "Full",
    "Empty",
    "Not found"
};

/** \test   test_mmp_error */
const char *mmp_error_ret_getdesc(ret_t ret)
{
    return error_strings[ret];
}

/** \todo   untested */
const char *mmp_error_err_getdesc(err_t err)
{
    return xstrerror(err);
}

