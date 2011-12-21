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
/** \file   mmp_vect.h
 * \brief   vect (variable array) datatypes and functions
 * \author  FtM
 * \date    2010-Oct-06
 */
#ifndef H_MMP_VECT_H
#define H_MMP_VECT_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief vect structure */
typedef struct mmp_vect_s {
    void **data;    /**< data vector */
    int n_data;     /**< current data count */
    int n_alloc;    /**< current allocated count */
} t_mmp_vect_s;

/** \brief id of a vect entry */
typedef int t_mmp_vect_id;
#define MMP_TABLE_INVALID_ID    -1

MMP_CEXTERN_BEGIN

/** \brief create a vect */
MMP_API t_mmp_vect_s *mmp_vect_create(int size);

/** \brief add an element a the bottom of the vect */
MMP_API t_mmp_vect_id mmp_vect_add(t_mmp_vect_s *vect, void *elem);

/** \brief delete an element from the vect */
MMP_API void *mmp_vect_del(t_mmp_vect_s *vect, t_mmp_vect_id id);

/** \brief destroy a vect */
MMP_API void mmp_vect_destroy(t_mmp_vect_s **vect);

/** \brief destroy a vect, and containing data too */
MMP_API void mmp_vect_destroy_withdata(t_mmp_vect_s **vect,
                                                    void(*cback)(void**));

MMP_CEXTERN_END

#endif /* H_MMP_VECT_H */

