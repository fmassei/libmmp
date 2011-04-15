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
/** \file   mmp_date.h
 * \brief   common date management functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_DATE_H
#define H_MMP_DATE_H

#ifndef _WIN32
#   ifndef _XOPEN_SOURCE
#      define _XOPEN_SOURCE
#   endif
#   ifndef _POSIX_C_SOURCE
#       define _POSIX_C_SOURCE 200112L
#   endif
#endif

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mmp_compat.h"
#include "mmp_trace.h"

MMP_CEXTERN_BEGIN

/** unix strptime wrapper
 * \todo missing unittest */
MMP_API char *xstrptime(const char *buf, const char *fmt, struct tm *tm);

/** format the current time in RFC-1123 format 
 * (Sun, 06 Nov 1994 08:49:37 GMT) */
MMP_API void mmp_time_1123_format(time_t t, char * datestr, size_t strsize);

/** format the current time in RFC-1036 format
 * (Sunday, 06-Nov-94 08:49:37 GMT) */
MMP_API void mmp_time_1036_format(time_t t, char * datestr, size_t strsize);

/** format the current time in asctime format */
MMP_API void mmp_time_asctime_format(time_t t, char * datestr, size_t strsize);

/** \brief parse the passed string into a date
 * try all the parsing between 1123, 1036 and asctime
 * \return MMP_ERR_PARSE if no available parsing where possible, MMP_ERR_OK
 *          otherwise. */
MMP_API ret_t mmp_parse_date(const char * __restrict date,
                             time_t * __restrict t);

/** \brief get timezone */
MMP_API char *mmp_get_tm_zone(void);
/** \brief set timezone */
MMP_API void mmp_set_tm_zone(char *zone);

MMP_CEXTERN_END

#endif /* H_DATE_H */
