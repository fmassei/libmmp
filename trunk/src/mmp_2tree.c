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
#include "mmp_2tree.h"

/** \test test_2tree */
t_mmp_2tree_s *mmp_2tree_create(void *data, t_mmp_2tree_s *l, t_mmp_2tree_s *r)
{
    t_mmp_2tree_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->data = data;
    ret->l = l;
    ret->r = r;
    return ret;
}

static void tree_destroy_singlenode(t_mmp_2tree_s** tree, void(*cback)(void**))
{
    if (tree==NULL || *tree==NULL) return;
    if (cback!=NULL && (*tree)->data!=NULL)
        cback(&(*tree)->data);
    xfree(*tree);
    *tree = NULL;
}

/** \test test_2tree */
void mmp_2tree_destroy(t_mmp_2tree_s** tree)
{
    if (tree==NULL || *tree==NULL) return;
    if ((*tree)->l!=NULL) mmp_2tree_destroy(&(*tree)->l);
    if ((*tree)->r!=NULL) mmp_2tree_destroy(&(*tree)->r);
    tree_destroy_singlenode(tree, NULL);
}

/** \test test_2tree */
void mmp_2tree_destroy_withdata(t_mmp_2tree_s **tree, void(*cback)(void**))
{
    if (tree==NULL || *tree==NULL) return;
    if ((*tree)->l!=NULL) mmp_2tree_destroy(&(*tree)->l);
    if ((*tree)->r!=NULL) mmp_2tree_destroy(&(*tree)->r);
    tree_destroy_singlenode(tree, cback);
}

/** \test test_2tree */
void mmp_2tree_visit_preorder(t_mmp_2tree_s *tree, t_mmp_2tree_vcallback fnc,
                                                                void *userdata)
{
    if (tree==NULL) return;
    if (fnc!=NULL) fnc(tree->data, userdata);
    if (tree->l!=NULL) mmp_2tree_visit_preorder(tree->l, fnc, userdata);
    if (tree->r!=NULL) mmp_2tree_visit_preorder(tree->r, fnc, userdata);
}

/** \test test_2tree */
void mmp_2tree_visit_inorder(t_mmp_2tree_s *tree, t_mmp_2tree_vcallback fnc,
                                                                void *userdata)
{
    if (tree==NULL) return;
    if (tree->l!=NULL) mmp_2tree_visit_inorder(tree->l, fnc, userdata);
    if (fnc!=NULL) fnc(tree->data, userdata);
    if (tree->r!=NULL) mmp_2tree_visit_inorder(tree->r, fnc, userdata);
}

/** \test test_2tree */
void mmp_2tree_visit_postorder(t_mmp_2tree_s *tree, t_mmp_2tree_vcallback fnc,
								void *userdata)
{
    if (tree==NULL) return;
    if (tree->l!=NULL) mmp_2tree_visit_postorder(tree->l, fnc, userdata);
    if (tree->r!=NULL) mmp_2tree_visit_postorder(tree->r, fnc, userdata);
    if (fnc!=NULL) fnc(tree->data, userdata);
}

