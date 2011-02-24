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

#ifdef UNIT_TESTING
/* creates a tree like:
 *       F
 *    /    \
 *   B      G
 *  / \      \
 * A   D      I
 *    / \    /
 *   C   E  H
 */
static t_mmp_2tree_s *build_test_tree(void)
{
    #define CNODE(_C) nodes[(_C)-'A']
    char *elems = "ABCDEFGHI";
    char el[2];
    t_mmp_2tree_s *nodes[9];
    int i;
    el[1] = '\0';
    for (i=0; i<9; ++i) {
        el[0] = elems[i];
        if ((nodes[i] = mmp_2tree_create(xstrdup(el), NULL, NULL))==NULL)
            return NULL;
    }
    CNODE('B')->l = CNODE('A'); CNODE('B')->r = CNODE('D');
    CNODE('F')->l = CNODE('B'); CNODE('F')->r = CNODE('G');
    CNODE('D')->l = CNODE('C'); CNODE('D')->r = CNODE('E');
    CNODE('G')->r = CNODE('I');
    CNODE('I')->l = CNODE('H');
    return CNODE('F');
    #undef CNODE    
}

static void free_el(void **el)
{
    xfree(*el);
    *el=NULL;
}

struct test_result_s {
    char buf[10];
    int buf_i;
};

static void visit(char *el, struct test_result_s *ts)
{
    ts->buf[ts->buf_i++] = el[0];
}
static void visit_v(void *data, void *userdata)
{
    visit((char *)data, (struct test_result_s*)userdata);
}

static t_mmp_tap_result_e test_2tree(void)
{
    t_mmp_2tree_s *tree;
    struct test_result_s ts;
    if ((tree = build_test_tree())==NULL)
        return MMP_TAP_FAILED;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_preorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "FBADCEGIH")!=0)
        return MMP_TAP_FAILED;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_inorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "ABCDEFGHI")!=0)
        return MMP_TAP_FAILED;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_postorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "ACEDBHIGF")!=0)
        return MMP_TAP_FAILED;
    mmp_2tree_destroy_withdata(&tree, free_el);
    if (tree!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

/* do the tests */
ret_t mmp_2tree_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp_2tree functions", NULL,
                                        test_2tree()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
