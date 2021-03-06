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
/** \file   mmp_htab.h
 * \brief   simple hashtable
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_HTAB_H
#define H_MMP_HTAB_H

#include "mmp_h_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_string.h"

/** \brief hashtable element */
typedef struct mmp_htab_elem_s {
    char *key;      /**< element key */
    void *val;      /**< element value */
    struct mmp_htab_elem_s *next;   /**< link to next elem */
    struct mmp_htab_elem_s *prev;   /**< link to prev elem */
} t_mmp_htab_elem_s;

/** \brief hashtable */
typedef struct mmp_htab_s {
    size_t size;                /**< size of hashtable */
    t_mmp_htab_elem_s **ptrs;   /**< element table */
} t_mmp_htab_s;

MMP_CEXTERN_BEGIN

/** \brief create an hastable */
MMP_API t_mmp_htab_s *mmp_htab_create(size_t size);

/** \brief destroy an hashtable */
MMP_API void mmp_htab_destroy(t_mmp_htab_s **ptr);

/** \brief destroy an hashtable, deleting each element with a callback */
MMP_API void mmp_htab_destroy_with_data(t_mmp_htab_s **ptr,
                                    void(*datadel)(void**));
/** \brief hash the key */
MMP_API unsigned int mmp_htab_hash(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict  str, int max);

/** \brief lookup for a partly-defined key */
MMP_API void *mmp_htab_lookup_nz(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, int keylen);

/** \brief lookup for a key */
MMP_API void *mmp_htab_lookup(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key);

/** \brief install an entry */
MMP_API ret_t mmp_htab_install(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, void *val);

/** \brief delete an entry */
MMP_API ret_t mmp_htab_delete(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key);

MMP_CEXTERN_END

#endif /* H_MMP_HTAB_H */
