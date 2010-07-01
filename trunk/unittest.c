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
#ifdef UNIT_TESTING
#include <stdio.h>
#include <stdlib.h>
#include "mmp_trace.h"
#include "mmp_tap.h"
#include "mmp_dir.h"
#include "mmp_error.h"
#include "mmp_string.h"
#include "mmp_list.h"
#include "mmp_htab.h"
#include "mmp_thread.h"
#include "mmp_files.h"

int main(void)
{
    t_mmp_tap_cycle_s *cycle;
    if ((cycle = mmp_tap_startcycle("mmp unit test"))==NULL) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    if (    (mmp_list_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_htab_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_dir_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_error_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_string_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_thread_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_files_unittest(cycle)!=MMP_ERR_OK) ||
            (mmp_tap_print(cycle, stdout)!=MMP_ERR_OK) ) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    mmp_tap_freecycle(&cycle);
#ifdef _WIN32
    system("PAUSE");
#endif
    return 0;
}
#else
static void stripme(void) {;}
#endif /* UNIT_TESTING */
