/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
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
    "Socket error"
    "Syncronization error",
    "Full",
    "Empty",
    "Not found"
};

/** \test   mmp_error_unittest */
const char * const mmp_error_ret_getdesc(ret_t ret)
{
    return error_strings[ret];
}

/** \todo   untested */
const char * const mmp_error_err_getdesc(err_t err)
{
    return xstrerror(err);
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_getdesc(void)
{
    if (!strcmp(mmp_error_ret_getdesc(MMP_ERR_SHM),"Shared memory error"))
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
ret_t mmp_error_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret = mmp_tap_test(cycle, "mmp_error_ret_getdesc", NULL,
                                                test_getdesc()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

