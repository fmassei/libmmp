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
/** \file mmp_dl.h
 * \brief   dynamic loading functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_DL_H
#define H_MMP_DL_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <dlfcn.h>
#else
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#endif

#include "mmp_compat.h"
#include "mmp_trace.h"
#include "mmp_funclist.h" /* for t_mmp_fp def */

MMP_CEXTERN_BEGIN

/** open a dinamically loadable library */
MMP_API t_mmp_fp mmp_dl_open_and_get_fnc(const char * __restrict filename,
                                      const char * __restrict getfnc_name);

MMP_CEXTERN_END

#endif /* H_MMP_DL_H */
