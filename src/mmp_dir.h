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
/**  \file  mmp_dir.h
 * \brief   common directory functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_DIR_H
#define H_MMP_DIR_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>

#ifndef _WIN32
#   include <sys/types.h>
#   include <dirent.h>
#else
#   include <windows.h>
#endif

#include "mmp_list.h"
#include "mmp_trace.h"
#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_compat.h"

/** list of file results */
typedef t_mmp_list_s t_mmp_dirlist_s;

MMP_CEXTERN_BEGIN

/** get file basename (unix basename) */
MMP_API char *mmp_basename(char *path);

/** list all files in a directory */
MMP_API t_mmp_dirlist_s *mmp_dir_listfiles(const char *dirname);

/** free the results of a mmp_dir_listfiles */
MMP_API void mmp_dir_free_list_results(t_mmp_dirlist_s **list);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
/** unit testing */
ret_t mmp_dir_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_DIR_H */
