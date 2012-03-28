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
/** \file   mmp_gen_vect.h
 * \brief   vect (variable array) datatypes and functions (generics)
 * \author  FtM
 * \date    2012-Mar-27
 */
#ifndef H_MMP_GEN_VECT_H
#define H_MMP_GEN_VECT_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_error.h"
#include "mmp_trace.h"

#define MMP_TD_VECT_PRE(_EXT) mmp_ ## _EXT ## _vect
#define MMP_TD_VECT_JOIN(_EXT) MMP_EVAL(MMP_TD_VECT_PRE(_EXT),_s)
#define MMP_TD_VECT_NAME(_EXT) MMP_EVAL(t_,MMP_TD_VECT_JOIN(_EXT))

#ifdef BUILDING_MMP
#   define MMP_FNC_LINKAGE MMP_API
#else
#   define MMP_FNC_LINKAGE
#endif

#define MMP_TD_VECT(_TYPE, _PRFX)   \
\
    typedef struct MMP_TD_VECT_JOIN(_PRFX) { \
        _TYPE *data; \
        size_t n_data; \
        size_t n_alloc; \
    } MMP_TD_VECT_NAME(_PRFX); \
\
MMP_CEXTERN_BEGIN \
\
MMP_FNC_LINKAGE MMP_TD_VECT_NAME(_PRFX) *\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_create)(size_t size, _TYPE val); \
\
MMP_FNC_LINKAGE void\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_fill)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, _TYPE val); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_reserve)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t size); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_resize)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t size, _TYPE val); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_push_back)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, _TYPE val); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_pop_back)\
            (MMP_TD_VECT_NAME(_PRFX) *vect); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_erase)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t pos); \
\
MMP_FNC_LINKAGE ret_t\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_insert)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t pos, _TYPE val); \
\
MMP_FNC_LINKAGE void \
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_destroy)\
            (MMP_TD_VECT_NAME(_PRFX) **vect); \
\
MMP_FNC_LINKAGE void \
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_destroy_with_data)\
            (MMP_TD_VECT_NAME(_PRFX) **vect, void(*back)(_TYPE*)); \
\
MMP_CEXTERN_END

/* use the define below to implement custom generic vects */
#define MMP_TD_VECT_IMPLEMENT(_TYPE, _PRFX) \
void MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_destroy)(MMP_TD_VECT_NAME(_PRFX) **vect)\
{\
    if (vect==NULL || *vect==NULL) return;\
    if ((*vect)->data!=NULL)\
        xfree((*vect)->data);\
    MMP_XFREE_AND_NULL(*vect);\
}\
\
void MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_destroy_with_data)\
        (MMP_TD_VECT_NAME(_PRFX) **vect, void(*cback)(_TYPE*))\
{\
    size_t i;\
    if (vect==NULL || *vect==NULL) return;\
    for (i=0; i<(*vect)->n_data; ++i)\
        cback(&(*vect)->data[i]);\
    if ((*vect)->data!=NULL)\
        xfree((*vect)->data);\
    MMP_XFREE_AND_NULL(*vect);\
}\
\
static size_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_nearSize)(size_t size)\
{/* FIXME: remove 4096 and use pagesize-something */\
    double reMax;\
    if (size==0) {\
        reMax = floor(4080.0/sizeof(_TYPE));\
        return (reMax==0) ? 1 : reMax;\
    } else if (sizeof(_TYPE)<4080) {\
        size_t oMem;\
        oMem = (size*sizeof(_TYPE))%4080;\
        reMax = (size*sizeof(_TYPE))+(4080-oMem);\
        return floor(reMax/size);\
    } else {\
        return size;\
    }\
}\
\
MMP_TD_VECT_NAME(_PRFX) *MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_create)\
    (size_t size, _TYPE val)\
{\
    MMP_TD_VECT_NAME(_PRFX) *ret;\
    MMP_XMALLOC_OR_RETURN(ret, NULL);\
    ret->n_alloc = MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_nearSize)(size);\
    if ((ret->data = xmalloc(sizeof(*ret->data)*ret->n_alloc))==NULL) {\
        mmp_setError(MMP_ERR_ENOMEM);\
        xfree(ret);\
        return NULL;\
    }\
    ret->n_data = size;\
    MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_fill)(ret, val);\
    return ret;\
}\
\
void MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_fill)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, _TYPE val)\
{\
    size_t i;\
    if (vect==NULL || vect->data==NULL) return;\
    for (i=0; i<vect->n_data; ++i)\
        vect->data[i] = val;\
}\
\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_reserve)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t size)\
{\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (vect->n_alloc>=size) {\
        return MMP_ERR_OK;\
    } else {\
        _TYPE *na;\
        if ((na = xrealloc(vect->data, sizeof(*na)*size))==NULL) {\
            mmp_setError(MMP_ERR_ENOMEM);\
            return MMP_ERR_ENOMEM;\
        }\
        vect->data = na;\
        vect->n_alloc = size;\
    }\
    return MMP_ERR_OK;\
}\
\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_resize)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t size, _TYPE val)\
{\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (vect->n_data>=size) {\
        vect->n_data = size;\
    } else {\
        size_t reMax, i;\
        reMax = MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_nearSize)(size);\
        if (MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_reserve)(vect,reMax)!=MMP_ERR_OK){\
            mmp_setError(MMP_ERR_GENERIC);\
            return MMP_ERR_GENERIC;\
        }\
        vect->n_alloc = reMax;\
        for (i=vect->n_data; i<size; ++i)\
            vect->data[i] = val;\
        vect->n_data = size;\
    }\
    return MMP_ERR_OK;\
}\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_push_back)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, _TYPE val)\
{\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (vect->n_alloc<vect->n_data+1) {\
        size_t reMax;\
        reMax = MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_nearSize)(vect->n_data+1);\
        if (MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_reserve)(vect,reMax)!=MMP_ERR_OK){\
            mmp_setError(MMP_ERR_GENERIC);\
            return MMP_ERR_GENERIC;\
        }\
    }\
    vect->data[vect->n_data++] = val;\
    return MMP_ERR_OK;\
}\
\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_pop_back)\
            (MMP_TD_VECT_NAME(_PRFX) *vect)\
{\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (vect->n_data==0)\
        return MMP_ERR_EMPTY;\
    --vect->n_data;\
    return MMP_ERR_OK;\
}\
\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_erase)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t pos)\
{\
    size_t i;\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (pos>=vect->n_data)\
        return MMP_ERR_EMPTY;\
    for (i=pos; i<vect->n_data; ++i)\
        vect->data[i] = vect->data[i+1];\
    --vect->n_data;\
    return MMP_ERR_OK;\
}\
\
ret_t MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_insert)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, size_t pos, _TYPE val)\
{\
    size_t i;\
    MMP_CHECK_OR_RETURN((vect!=NULL), MMP_ERR_PARAMS);\
    if (pos>=vect->n_data)\
        return MMP_ERR_EMPTY;\
    if (vect->n_data+1>vect->n_alloc) {\
        size_t reMax;\
        reMax = MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_nearSize)(vect->n_data+1);\
        if (MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_reserve)(vect,reMax)!=MMP_ERR_OK){\
            mmp_setError(MMP_ERR_GENERIC);\
            return MMP_ERR_GENERIC;\
        }\
    }\
    for (i=vect->n_data; i>pos; --i)\
        vect->data[i] = vect->data[i-1];\
    vect->data[pos] = val;\
    ++vect->n_data;\
    return MMP_ERR_OK;\
}

MMP_TD_VECT(int, i)
MMP_TD_VECT(unsigned int, ui)
MMP_TD_VECT(double, d)
MMP_TD_VECT(void*, v)

#endif /* H_MMP_VECT_H */

