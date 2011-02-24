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
/** \file   mmp_2tree.h
 * \brief   binary tree datatypes and functions
 * \author  FtM
 * \date    2010-Sep-08
 */
#ifndef H_MMP_2TREE_H
#define H_MMP_2TREE_H

#include <stdio.h>
#include <stdlib.h>
#include "mmp_h_utils.h"
#include "mmp_compat.h"
#include "mmp_memory.h"
#include "mmp_trace.h"

/** \brief binary tree structure */
typedef struct mmp_2tree_s {
    void *data;
    struct mmp_2tree_s *l, *r;
} t_mmp_2tree_s;

/** \brief 2tree visit callback (void *data, void *userdata) */
typedef void(*t_mmp_2tree_vcallback)(void *, void *);

MMP_CEXTERN_BEGIN

/** \brief create a tree */
MMP_API t_mmp_2tree_s *mmp_2tree_create(void *data, t_mmp_2tree_s *l,
                                                    t_mmp_2tree_s *r);

/** \brief destroy a tree */
MMP_API void mmp_2tree_destroy(t_mmp_2tree_s** tree);

/** \brief destroy a tree with data */
MMP_API void mmp_2tree_destroy_withdata(t_mmp_2tree_s **tree,
                                                    void(*cback)(void**));

/** \brief visit a tree in preorder */
MMP_API void mmp_2tree_visit_preorder(t_mmp_2tree_s *tree,
                                                    t_mmp_2tree_vcallback fnc,
                                                    void *userdata);

/** \brief visit a tree in inorder */
MMP_API void mmp_2tree_visit_inorder(t_mmp_2tree_s *tree,
                                                    t_mmp_2tree_vcallback fnc,
                                                    void *userdata);

/** \brief visit a tree in postorder */
MMP_API void mmp_2tree_visit_postorder(t_mmp_2tree_s *tree,
                                                    t_mmp_2tree_vcallback fnc,
                                                    void *userdata);

MMP_CEXTERN_END

#ifdef UNIT_TESTING
#include "mmp_tap.h"
ret_t mmp_2tree_unittest(t_mmp_tap_cycle_s *cycle);
#endif /* UNIT_TESTING */

#endif /* H_MMP_2TREE_H */
