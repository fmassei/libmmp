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
#include <stdio.h>
#include <stdlib.h>
#include "mmp_queue.h"

int main (void)
{
    t_mmp_queue_s *queue;
    ret_t res;
    int i = 0, *dt;
    void *pdt;
    if ((queue = mmp_queue_create(10))==NULL)
        return EXIT_FAILURE;
    for (i=1; ; ++i) {
        if ((dt = xmalloc(sizeof(*dt)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return EXIT_FAILURE;
        }
        *dt = i;
        res = mmp_queue_enqueue(queue, dt);
        if (res==MMP_ERR_FULL) break;
        else if (res==MMP_ERR_OK) continue;
        else return EXIT_FAILURE;
    }
    for (i=1; i<10; ++i) {
        res = mmp_queue_dequeue(queue, &pdt);
        dt = (int*)pdt;
        if (*dt!=i)
            return EXIT_FAILURE;
        xfree(dt);
    }
    mmp_queue_destroy(&queue);
    if (queue!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

