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
/** \file   mmp_barray.h
 * \brief   paged indexed array datatypes and functions
 * \author  FtM
 * \date    2010-Aug-23
 */
#ifndef H_MMP_BARRAY_H
#define H_MMP_BARRAY_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_mmap.h"
#include "mmp_files.h"

/** \brief barray index datatype */
typedef unsigned int t_mmp_barray_idx;

/** \brief barray struct */
typedef struct mmp_barray_s t_mmp_barray_s;

MMP_CEXTERN_BEGIN

/** \brief create a barray */
MMP_API t_mmp_barray_s *mmp_barray_create(const char *fname,
                                            unsigned int page_size,
                                            unsigned int data_size,
                                            unsigned int max_cache_entries);

/** \brief insert data into a barray */
MMP_API ret_t mmp_barray_insert(t_mmp_barray_s * __restrict barray,
                        t_mmp_barray_idx idx, const void * __restrict data);

/** \brief search the barray for a given index */
MMP_API ret_t mmp_barray_search(t_mmp_barray_s * __restrict barray,
                        t_mmp_barray_idx idx, void ** __restrict data);

/** \brief delete the barray data at a given index */
MMP_API ret_t mmp_barray_delete(t_mmp_barray_s *barray, t_mmp_barray_idx idx);

/** \brief destroy the barray */
MMP_API void mmp_barray_destroy(t_mmp_barray_s **barray);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_barray_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_BARRAY_H */
