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
/** \file   mmp_queue.h
 * \brief   FIFO queue datatypes and functions
 * \author  FtM
 * \date    2011-Mar-11
 */
#ifndef H_MMP_QUEUE_H
#define H_MMP_QUEUE_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief queue structure */
typedef struct mmp_queue_s {
    void **data;    /**< data vector */
    int max_size;   /**< max vector size */
    int n_data;     /**< current data count */
    int from;       /**< initial pointer */
} t_mmp_queue_s;

MMP_CEXTERN_BEGIN

/** \brief create a queue */
MMP_API t_mmp_queue_s *mmp_queue_create(int size);

/** \brief enqueue a pointer into the queue (or return MMP_ERR_FULL) */
MMP_API ret_t mmp_queue_enqueue(t_mmp_queue_s *queue, void *elem);

/** \brief dequeue a pointer from the queue (or return MMP_ERR_EMPTY) */
MMP_API ret_t mmp_queue_dequeue(t_mmp_queue_s *queue, void **elem);

/** \brief destroy a queue */
MMP_API void mmp_queue_destroy(t_mmp_queue_s **queue);

MMP_CEXTERN_END

#endif /* H_MMP_QUEUE_H */

