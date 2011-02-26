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
/** \file   mmp_list.h
 * \brief   double-linked list datatypes and functions
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_LIST_H
#define H_MMP_LIST_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief a list element */
typedef struct mmp_listelem_s {
    void *data;                     /**< element data */
    struct mmp_listelem_s *next;    /**< next element */
    struct mmp_listelem_s *prev;    /**< previous element */
} t_mmp_listelem_s;

/** \brief a list */
typedef struct mmp_list_s {
    int nelems;                     /**< number of elements */
    t_mmp_listelem_s *head;         /**< pointer to head element */
    t_mmp_listelem_s *tail;         /**< pointer to tail element */
} t_mmp_list_s;

/** \brief a comparer function */
typedef int(* t_mmp_comparer_f)(const void * const, const void * const);

MMP_CEXTERN_BEGIN

/** \brief creates an empty list */
MMP_API t_mmp_list_s *mmp_list_create(void);

/** \brief delete a complete list */
MMP_API void mmp_list_delete(t_mmp_list_s **list);

/** \brief delete a complete list calling a function on each data before
 * deletion */
MMP_API void mmp_list_delete_withdata(t_mmp_list_s **list, 
                                        void(*cback)(void**));

/** \brief add data to the list */
MMP_API ret_t mmp_list_add_data(t_mmp_list_s * __restrict list, void * data);

/** \brief add sorted data to the list */
MMP_API ret_t mmp_list_add_data_sorted(t_mmp_list_s * __restrict list,
                                        void *data, t_mmp_comparer_f comp);

/** \brief delete an element from the list, return the element data */
MMP_API void *mmp_list_del_elem(t_mmp_list_s * __restrict list,
                                        t_mmp_listelem_s **elem);

/** \brief delete an element from the list, by data */
MMP_API void* mmp_list_del_elem_by_data(t_mmp_list_s * __restrict list,
                                        const void * data);

/** \brief find data in the list */
MMP_API t_mmp_listelem_s *mmp_list_find_data(
                                        const t_mmp_list_s * __restrict list,
                                        const void * data);

/** \brief find data in the list by comparer lambda */
MMP_API t_mmp_listelem_s *mmp_list_find_data_lambda(
                                        const t_mmp_list_s * __restrict list,
                                        const void * data,
                                        t_mmp_comparer_f comp);

/** \brief find data in the list by comparer lambda, supposing that data is
 * sorted */
MMP_API t_mmp_listelem_s *mmp_list_find_data_sorted(
                                        const t_mmp_list_s * __restrict list,
                                        const void * data,
                                        t_mmp_comparer_f comp);

/** \brief execute a lambda function for each element in list */
MMP_API void mmp_list_lambda_elem(t_mmp_list_s * __restrict list,
                                        void(*fnc)(t_mmp_listelem_s *));

/** \brief execute a lambda function for each element data in list */
MMP_API void mmp_list_lambda_data(t_mmp_list_s * __restrict list,
                                        void(*fnc)(void*));

/** \brief execute a lambda function for each element data in list, with extra
 * params*/
MMP_API void mmp_list_lambda_data_ext(t_mmp_list_s * __restrict list,
                                        void(*fnc)(void*, void*),
                                        void *params);

/** \brief execute a lambda function for each element data in list, with extra
 * params*/
MMP_API void mmp_list_lambda_data_ext_rev(t_mmp_list_s * __restrict list,
                                        void(*fnc)(void*, void*),
                                        void *params);

/** \brief swap two list elements */
MMP_API void mmp_list_swap_elems(t_mmp_listelem_s * e1, t_mmp_listelem_s * e2);

/** \brief sory a list by elements, with a comparer lambda */
MMP_API void mmp_list_sort_by_data(t_mmp_list_s * __restrict list,
                                        t_mmp_comparer_f comp);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_list_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_LIST_H */
