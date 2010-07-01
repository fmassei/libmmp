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
#include "mmp_files.h"

int mmp_stat(const char *path, t_mmp_stat_s *stat_ptr)
{
#ifndef _WIN32
    return stat(path, stat_ptr);
#else
    return _stat(path, stat_ptr);
#endif
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_stat(void)
{
    return MMP_TAP_UNTESTED;
}

ret_t mmp_file_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret = mmp_tap_test(cycle, "mmp_stat", "unimplemented",
                                                test_stat()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
