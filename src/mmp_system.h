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
/** \file   mmp_system.h
 * \brief   system settings functions wrappers
 * \author  FtM
 * \date    2010-Aug-24
 */
#ifndef H_MMP_SYSTEM_H
#define H_MMP_SYSTEM_H

#include "mmp_h_utils.h"
#include <stdio.h>
#ifndef _WIN32
#   include <unistd.h>
#else
#   include <windows.h>
#endif

MMP_CEXTERN_BEGIN

/** \brief get the system page size */
MMP_API long mmp_system_getPageSize(void);

/** \brief get the system page alignment */
MMP_API long mmp_system_getPageAlignment(void);

MMP_CEXTERN_END

#endif /* H_MMP_SYSTEM_H */
