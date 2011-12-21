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
/** \file   mmp_table.h
 * \brief   table (variable array) datatypes and functions
 * \author  FtM
 * \date    2010-Oct-06
 */
#ifndef H_MMP_TABLE_H
#define H_MMP_TABLE_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief table structure */
typedef struct mmp_table_s {
    void **data;    /**< data vector */
    int n_data;     /**< current data count */
    int n_alloc;    /**< current allocated count */
} t_mmp_table_s;

/** \brief id of a table entry */
typedef int t_mmp_table_id;
#define MMP_TABLE_INVALID_ID    -1

MMP_CEXTERN_BEGIN

/** \brief create a table */
MMP_API t_mmp_table_s *mmp_table_create(int size);

/** \brief add an element a the bottom of the table */
MMP_API t_mmp_table_id mmp_table_add(t_mmp_table_s *table, void *elem);

/** \brief delete an element from the table */
MMP_API void *mmp_table_del(t_mmp_table_s *table, t_mmp_table_id id);

/** \brief destroy a table */
MMP_API void mmp_table_destroy(t_mmp_table_s **table);

/** \brief destroy a table, and containing data too */
MMP_API void mmp_table_destroy_withdata(t_mmp_table_s **table,
                                                    void(*cback)(void**));

MMP_CEXTERN_END

#endif /* H_MMP_STACK_H */

