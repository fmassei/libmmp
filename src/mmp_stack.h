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
/** \file   mmp_stack.h
 * \brief   LIFO stack datatypes and functions
 * \author  FtM
 * \date    2010-Aug-27
 */
#ifndef H_MMP_STACK_H
#define H_MMP_STACK_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief stack structure */
typedef struct mmp_stack_s {
    void **data;    /**< data vector */
    int max_size;   /**< max vector size */
    int n_data;     /**< current data count */
} t_mmp_stack_s;

MMP_CEXTERN_BEGIN

/** \brief create a stack */
MMP_API t_mmp_stack_s *mmp_stack_create(int size);

/** \brief push a pointer into the stack (or return MMP_ERR_FULL) */
MMP_API ret_t mmp_stack_push(t_mmp_stack_s *stack, void *elem);

/** \brief pop a pointer from the stack (or return MMP_ERR_EMPTY) */
MMP_API ret_t mmp_stack_pop(t_mmp_stack_s *stack, void **elem);

/** \brief destroy a stack */
MMP_API void mmp_stack_destroy(t_mmp_stack_s **stack);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_stack_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_STACK_H */

