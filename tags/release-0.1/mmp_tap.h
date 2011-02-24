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
/** \file   mmp_tap.h
 * \brief   tap testing functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_TAP_H
#define H_MMP_TAP_H

#include <stdio.h>
#include <stdlib.h>

/** \brief tap results */
typedef enum mmp_tap_result_e {
    MMP_TAP_PASSED,
    MMP_TAP_FAILED,
    MMP_TAP_UNTESTED
} t_mmp_tap_result_e;

/** \brief a tap entry */
typedef struct mmp_tap_entry_s {
    t_mmp_tap_result_e result;      /**< result of the test */
    char *desc;                     /**< description on the test */
    char *comment;                  /**< additional comments */
} t_mmp_tap_entry_s;

/** \brief a tap testing cycle */
typedef struct mmp_tap_cycle_s {
    char *name;                     /**< name of the test cycle */
    struct mmp_list_s *tests;       /**< list of test entries */
} t_mmp_tap_cycle_s;

#include "mmp_h_utils.h"
#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_list.h"
#include "mmp_trace.h"

MMP_CEXTERN_BEGIN

/** \brief start a tap cycle */
MMP_API t_mmp_tap_cycle_s *mmp_tap_startcycle(const char *name);

/** \brief do a test */
MMP_API ret_t mmp_tap_test(t_mmp_tap_cycle_s * __restrict cycle,
                    const char * __restrict desc,
                    const char * __restrict comment, t_mmp_tap_result_e res);

/** \brief the test passed */
MMP_API ret_t mmp_tap_test_passed(t_mmp_tap_cycle_s * __restrict cycle,
                    const char * __restrict desc,
                    const char * __restrict comment);

/** \brief the test failed */
MMP_API ret_t mmp_tap_test_failed(t_mmp_tap_cycle_s * __restrict cycle,
                    const char * __restrict desc,
                    const char * __restrict comment);

/** \brief the test was skipped */
MMP_API ret_t mmp_tap_test_skipped(t_mmp_tap_cycle_s * __restrict cycle,
                    const char * __restrict desc,
                    const char * __restrict comment);

/** \brief free the cycle results */
MMP_API void mmp_tap_freecycle(t_mmp_tap_cycle_s **cycle);

/** \brief print results */
MMP_API ret_t mmp_tap_print(t_mmp_tap_cycle_s *cycle, FILE *out);

MMP_CEXTERN_END

#endif /* H_MMP_TAP_H */
