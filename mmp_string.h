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
/** \file   mmp_string.h
 * \brief   string functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_STRING_H
#define H_MMP_STRING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _WIN32
#   include <strings.h>
#endif
#include "mmp_h_utils.h"
#include "mmp_error.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

MMP_CEXTERN_BEGIN

/** \brief unix strdup() */
MMP_API char *xstrdup(const char *s);

/** \brief strdup() with n characters */
MMP_API char *xstrdupn(const char *s, size_t n);

/** \brief unix index() */
MMP_API char *xindex(const char *s, char c);

/** \brief unix rindex() */
MMP_API char *xrindex(const char *s, char c);

/** \brief unix strncasecmp() */
MMP_API int xstrncasecmp(const char *s1, const char *s2, size_t n);

/** \brief GNU strtok_r() */
MMP_API char *xstrtok_r(char *str, const char *delim, char **ctx);


/** \brief is a character "trimmable"? */
MMP_API int mmp_str_is_trimmable(char c);

/** \brief left trim */
MMP_API char *mmp_str_ltrim(const char *str);

/** \brief right trim */
MMP_API char *mmp_str_rtrim(char *str);

/** \brief left and right trim */
MMP_API char *mmp_str_trim(char *str);


/** \brief convert to windows-style path */
MMP_API char *mmp_str_pathUnixToSys(const char *str);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_string_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_STRING_H */
