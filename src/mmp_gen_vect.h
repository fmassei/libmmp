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
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_vect.h"

#define MMP_TD_VECT_PRE(_EXT) mmp_ ## _EXT ## vect
#define MMP_TD_VECT_JOIN(_EXT) MMP_EVAL(MMP_TD_VECT_PRE(_EXT),_s)
#define MMP_TD_VECT_NAME(_EXT) MMP_EVAL(t_,MMP_TD_VECT_JOIN(_EXT))
#define MMP_TD_VECT_ID(_EXT) MMP_EVAL(MMP_EVAL(t_,MMP_TD_VECT_PRE(_EXT)),_id)

#define MMP_TD_VECT(_TYPE, _PRFX)   \
    typedef int MMP_TD_VECT_ID(_PRFX); \
\
    typedef struct MMP_TD_VECT_JOIN(_PRFX) { \
        _TYPE *data; \
        int n_data; \
        int n_alloc; \
    } MMP_TD_VECT_NAME(_PRFX); \
\
MMP_CEXTERN_BEGIN \
\
MMP_API MMP_TD_VECT_NAME(_PRFX) *\
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_create)(int size); \
\
MMP_API MMP_TD_VECT_ID(_PRFX) \
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_push)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, _TYPE elem); \
\
MMP_API _TYPE \
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_del)\
            (MMP_TD_VECT_NAME(_PRFX) *vect, MMP_TD_VECT_ID(_PRFX) id); \
\
MMP_API void \
            MMP_EVAL(MMP_TD_VECT_PRE(_PRFX),_destroy)\
            (MMP_TD_VECT_NAME(_PRFX) **vect); \
\
MMP_CEXTERN_END

MMP_TD_VECT(int, i)
MMP_TD_VECT(unsigned int, ui)
MMP_TD_VECT(double, d)

#endif /* H_MMP_VECT_H */

