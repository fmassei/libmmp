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
/** \file   mmp_getopt.h
 * \brief   getopt function
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_GETOPT_H
#define H_MMP_GETOPT_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <getopt.h>
#else
#   include "mmp_compat.h"
#   include "mmp_string.h"

    extern int opterr;
    extern int optind;
    extern int optopt;
    extern char *optarg;

#endif

MMP_CEXTERN_BEGIN

/** unix getopt */
MMP_API int xgetopt(const int argc, char * const *argv, char *opts);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_getopt_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_GETOPT_H */
