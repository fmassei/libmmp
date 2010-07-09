/*
    Copyright 2010 Francesco Massei

    This file is part of the MMP library.

        MMP is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MMP.  If not, see <http://www.gnu.org/licenses/>.
*/
/** \file   mmp_htab.h
 * \brief   simple hashtable
 * \author  FtM
 * \date    2010-Jul-08
 */
#ifndef H_MMP_HTAB_H
#define H_MMP_HTAB_H

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "mmp_memory.h"
#include "mmp_trace.h"
#include "mmp_string.h"

/** hashtable element */
typedef struct mmp_htab_elem_s {
    char *key;
    void *val;
    struct mmp_htab_elem_s *next, *prev;
} t_mmp_htab_elem_s;

/** hashtable */
typedef struct mmp_htab_s {
    size_t size;
    t_mmp_htab_elem_s **ptrs;
} t_mmp_htab_s;

/** create an hastable */
t_mmp_htab_s *mmp_htab_create(size_t size);
/** destroy an hashtable */
void mmp_htab_destroy(t_mmp_htab_s **ptr);
/** destroy an hashtable, deleting each element with a callback */
void mmp_htab_destroy_with_data(t_mmp_htab_s **ptr, void(*datadel)(void**));
/** hash the key */
unsigned int mmp_htab_hash(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict  str, int max);
/** lookup for a partly-defined key */
void *mmp_htab_lookup_nz(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, int keylen);
/** lookup for a key */
void *mmp_htab_lookup(const t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key);
/** install an entry */
ret_t mmp_htab_install(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key, void *val);
/** delete an entry */
ret_t mmp_htab_delete(t_mmp_htab_s * __restrict htab,
                                    const char * __restrict key);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_htab_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_HTAB_H */
