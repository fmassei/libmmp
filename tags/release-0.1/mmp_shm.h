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
/** \file   mmp_shm.h
 * \brief   shared memory functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_SHM_H
#define H_MMP_SHM_H

/* shared memory functions */

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#   include <windows.h>
#   include <winnt.h>
#   include <memory.h>
    typedef HANDLE t_mmp_shm_mapf;
#else
#   include <unistd.h>
#   include <sys/types.h>
#   include <sys/mman.h>
#   include <sys/stat.h>
#   include <fcntl.h>
    typedef int t_mmp_shm_mapf;
#endif
#include "mmp_h_utils.h"
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_string.h"
#include "mmp_trace.h"

/** \typedef t_mmp_shm_mapf
 * \brief share memory object */

/** \brief a shared memory object */
typedef struct mmp_shm_s {
    void *base;             /**< base address */
    size_t len;             /**< memory block length */
    char *filename;         /**< associated filename */
    t_mmp_shm_mapf map;     /**< shared map object */
} t_mmp_shm_s;

MMP_CEXTERN_BEGIN

/** \brief create and attach a shared memory */
MMP_API t_mmp_shm_s *mmp_shm_attach(const char * name, size_t size);

/** \brief put data on the shared memory */
MMP_API ret_t mmp_shm_getdata(const t_mmp_shm_s * __restrict shm,
                        void * __restrict dst, size_t from, size_t len);

/** \brief get data from the shared memory */
MMP_API ret_t mmp_shm_putdata(const t_mmp_shm_s * __restrict shm,
                        const void * __restrict src, size_t from, size_t len);

/** \brief detach and destroy the shared memory */
MMP_API void mmp_shm_detach(t_mmp_shm_s **map);

MMP_CEXTERN_END

#endif /* H_MMP_SHM_H */
