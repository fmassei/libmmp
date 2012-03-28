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
/** \file   mmp_gen_array.h
 * \brief   array datatypes and functions (generics)
 * \author  FtM
 * \date    2012-Mar-28
 */
#ifndef H_MMP_GEN_ARRAY_H
#define H_MMP_GEN_ARRAY_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

#define MMP_TD_ARRAY_PRE(_EXT) mmp_ ## _EXT ## _array
#define MMP_TD_ARRAY_JOIN(_EXT) MMP_EVAL(MMP_TD_ARRAY_PRE(_EXT),_s)
#define MMP_TD_ARRAY_NAME(_EXT) MMP_EVAL(t_,MMP_TD_ARRAY_JOIN(_EXT))

#ifdef BUILDING_MMP
#   define MMP_FNC_LINKAGE MMP_API
#else
#   define MMP_FNC_LINKAGE
#endif

#define MMP_TD_ARRAY(_TYPE, _PRFX)   \
\
    typedef struct MMP_TD_ARRAY_JOIN(_PRFX) { \
        _TYPE *data; \
        size_t n_data; \
    } MMP_TD_ARRAY_NAME(_PRFX); \
\
MMP_CEXTERN_BEGIN \
\
MMP_FNC_LINKAGE MMP_TD_ARRAY_NAME(_PRFX) *\
            MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_create)(int size); \
\
MMP_FNC_LINKAGE void \
            MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_destroy)\
            (MMP_TD_ARRAY_NAME(_PRFX) **array); \
\
MMP_FNC_LINKAGE void \
            MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_fill)\
            (MMP_TD_ARRAY_NAME(_PRFX) *array, _TYPE val); \
\
MMP_FNC_LINKAGE void \
            MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_destroy_with_data)\
            (MMP_TD_ARRAY_NAME(_PRFX) **array, void(*cback)(_TYPE*)); \
\
MMP_CEXTERN_END

/* use the define below to implement custom generic vects */
#define MMP_TD_ARRAY_IMPLEMENT(_TYPE, _PRFX) \
void MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_destroy)\
    (MMP_TD_ARRAY_NAME(_PRFX) **array)\
{\
    if (array==NULL || *array==NULL) return;\
    if ((*array)->data!=NULL)\
        xfree((*array)->data);\
    MMP_XFREE_AND_NULL(*array);\
}\
\
void MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_destroy_with_data)\
    (MMP_TD_ARRAY_NAME(_PRFX) **array, void(*cback)(_TYPE*))\
{\
    size_t i;\
    if (array==NULL || *array==NULL) return;\
    for (i=0; i<(*array)->n_data; ++i)\
        cback(&(*array)->data[i]);\
    if ((*array)->data!=NULL)\
        xfree((*array)->data);\
    MMP_XFREE_AND_NULL(*array);\
}\
\
MMP_TD_ARRAY_NAME(_PRFX) *MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_create)(int size)\
{\
    MMP_TD_ARRAY_NAME(_PRFX) *ret;\
    MMP_XMALLOC_OR_RETURN(ret, NULL);\
    if ((ret->data = xmalloc(sizeof(*ret->data)*size))==NULL) {\
        mmp_setError(MMP_ERR_ENOMEM);\
        xfree(ret);\
        return NULL;\
    }\
    ret->n_data = size;\
    return ret;\
}\
void MMP_EVAL(MMP_TD_ARRAY_PRE(_PRFX),_fill)\
    (MMP_TD_ARRAY_NAME(_PRFX) *array, _TYPE val)\
{\
    size_t i;\
    if (array==NULL) return;\
    for (i=0; i<array->n_data; ++i)\
        array->data[i] = val;\
}

MMP_TD_ARRAY(int, i)
MMP_TD_ARRAY(unsigned int, ui)
MMP_TD_ARRAY(double, d)
MMP_TD_ARRAY(void*, v)

#endif /* H_MMP_GEN_ARRAY_H */

