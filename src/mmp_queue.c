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
#include "mmp_queue.h"

/** \test test_queue */
void mmp_queue_destroy(t_mmp_queue_s **queue)
{
    if (queue==NULL || *queue==NULL) return;
    if ((*queue)->data!=NULL)
        xfree((*queue)->data);
    MMP_XFREE_AND_NULL(*queue);
}

/** \test test_queue */
t_mmp_queue_s *mmp_queue_create(int size)
{
    t_mmp_queue_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    if ((ret->data = xmalloc(sizeof(*ret->data)*size))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(ret);
        return NULL;
    }
    ret->max_size = size;
    ret->n_data = ret->from = 0;
    return ret;
}

/** \test test_queue */
ret_t mmp_queue_enqueue(t_mmp_queue_s *queue, void *elem)
{
    int ptr;
    MMP_CHECK_OR_RETURN((queue!=NULL && queue->data!=NULL), MMP_ERR_PARAMS);
    if (queue->n_data>=queue->max_size)
        return MMP_ERR_FULL;
    if ((queue->from+queue->n_data)>=queue->max_size)
        ptr = queue->max_size-queue->n_data-1;
    else
        ptr = queue->from+queue->n_data;
    queue->data[ptr] = elem;
    ++queue->n_data;
    return MMP_ERR_OK;
}

/** \test test_queue */
ret_t mmp_queue_dequeue(t_mmp_queue_s *queue, void **elem)
{
    MMP_CHECK_OR_RETURN((queue!=NULL && queue->data!=NULL && elem!=NULL),
                        MMP_ERR_PARAMS);
    if (queue->n_data<=0)
        return MMP_ERR_EMPTY;
    *elem = queue->data[queue->from];
    ++queue->from;
    if (queue->from>=queue->max_size)
        queue->from = 0;
    --queue->n_data;
    return MMP_ERR_OK;
}

