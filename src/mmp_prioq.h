/*
    Copyright 2011 Francesco Massei

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
/** \file   mmp_prioq.h
 * \brief   implementation of a priority queue
 * \author  FtM
 * \date    2011-Dec-29
 */
#ifndef H_MMP_PRIOQ_H
#define H_MMP_PRIOQ_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_error.h"

typedef struct mmp_prioq_s t_mmp_prioq_s;

MMP_CEXTERN_BEGIN

/** \brief create a priority queue */
MMP_API t_mmp_prioq_s *mmp_prioq_create(size_t prealloc);

/** \brief destroy a priority queue */
MMP_API void mmp_prioq_destroy(t_mmp_prioq_s **prioq);

/** \brief destroy a priority queue, applying a callback to each element */
MMP_API void mmp_prioq_destroy_withdata(t_mmp_prioq_s **prioq,
                                                        void(*cback)(void**));

/** \brief insert an element with a priority */
MMP_API ret_t mmp_prioq_insert(t_mmp_prioq_s *prioq, void *data, int prio);

/** \brief pop the highest priority element */
MMP_API ret_t mmp_prioq_pop(t_mmp_prioq_s *prioq, void **data, int *prio);

MMP_CEXTERN_END

#endif /* H_MMP_PRIOQ_H */

