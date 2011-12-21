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
/** \file   mmp_funclist.h
 * \brief   double-linked list (for functions) datatypes and functions
 * \author  FtM
 * \date    2010-Oct-27
 */
#ifndef H_MMP_FUNCLIST_H
#define H_MMP_FUNCLIST_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief generic function pointer */
typedef void(*t_mmp_fp)(void);

/** \brief a funclist element */
typedef struct mmp_funclistelem_s {
    t_mmp_fp data;                      /**< element data */
    struct mmp_funclistelem_s *next;    /**< next element */
    struct mmp_funclistelem_s *prev;    /**< previous element */
} t_mmp_funclistelem_s;

/** \brief a funclist */
typedef struct mmp_funclist_s {
    int nelems;                         /**< number of elements */
    t_mmp_funclistelem_s *head;         /**< pointer to head element */
    t_mmp_funclistelem_s *tail;         /**< pointer to tail element */
} t_mmp_funclist_s;

MMP_CEXTERN_BEGIN

/** \brief creates an empty list */
MMP_API t_mmp_funclist_s *mmp_funclist_create(void);

/** \brief delete a complete list */
MMP_API void mmp_funclist_destroy(t_mmp_funclist_s **list);

/** \brief add data to the list */
MMP_API ret_t mmp_funclist_add_func(t_mmp_funclist_s * __restrict list,
                                        t_mmp_fp func);

/** \brief delete an element from the list, return the element data */
MMP_API void *mmp_funclist_del_elem(t_mmp_funclist_s * __restrict list,
                                        t_mmp_funclistelem_s **elem);

/** \brief delete an element from the list, by data */
MMP_API void* mmp_funclist_del_elem_by_func(t_mmp_funclist_s * __restrict list,
                                        const t_mmp_fp data);

/** \brief find data in the list */
MMP_API t_mmp_funclistelem_s *mmp_funclist_find_func(
                                        const t_mmp_funclist_s* __restrict list,
                                        const t_mmp_fp data);

/** \brief execute a lambda function for each element in list */
MMP_API void mmp_funclist_lambda_elem(t_mmp_funclist_s * __restrict list,
                                        void(*fnc)(t_mmp_funclistelem_s *));

/** \brief execute a lambda function for each element data in list */
MMP_API void mmp_funclist_lambda_func(t_mmp_funclist_s * __restrict list,
                                        void(*fnc)(t_mmp_fp));

/** \brief execute a lambda function for each element data in list, with extra
 * params*/
MMP_API void mmp_funclist_lambda_func_ext(t_mmp_funclist_s * __restrict list,
                                        void(*fnc)(t_mmp_fp, void*),
                                        void *params);

/** \brief execute a lambda function for each element data in list, with extra
 * params*/
MMP_API void mmp_funclist_lambda_func_ext_rev(t_mmp_funclist_s* __restrict list,
                                        void(*fnc)(t_mmp_fp, void*),
                                        void *params);

/** \brief swap two list elements */
MMP_API void mmp_funclist_swap_elems(t_mmp_funclistelem_s * e1,
                                        t_mmp_funclistelem_s * e2);

MMP_CEXTERN_END

#endif /* H_MMP_FUNCLIST_H */
