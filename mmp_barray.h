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
/** \file   mmp_barray.h
 * \brief   paged indexed array datatypes and functions
 * \author  FtM
 * \date    2010-Aug-23
 */
#ifndef H_MMP_BARRAY_H
#define H_MMP_BARRAY_H

#include <stdio.h>
#include <stdlib.h>
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

typedef unsigned int t_mmp_barray_idx;

typedef struct mmp_barray_rec_s {
    uint8_t present:1;
    t_mmp_barray_idx idx:(sizeof(t_mmp_barray_idx)*8-1);
    void *data;
} t_mmp_barray_rec_s;

typedef struct mmp_barray_page_s {
    t_mmp_barray_rec_s *records;
    uint8_t dirty:1;
    uint16_t n_data:(sizeof(uint16_t)*8-1);
    t_mmp_barray_idx start, end;
} t_mmp_barray_page_s;

typedef struct mmp_barray_s {
    int fd;
    t_mmp_barray_page_s **pages;
    unsigned int n_pages, page_len, recs_per_page;
} t_mmp_barray_s;

t_mmp_barray_st *mmp_barray_create(const char *fname);
ret_t mmp_barray_insert(t_mmp_barray_s *barray, t_mmp_barray_idx idx,
                        void *data);
ret_t mmp_barray_search(t_mmp_barray_s *barray, t_mmp_barray_idx idx,
                        void *data);
ret_t mmp_barray_delete(t_mmp_barray_s *barray, t_mmp_barray_idx idx);
void mmp_barray_destroy(t_mmp_barray_s **barray);

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_barray_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_BARRAY_H */
