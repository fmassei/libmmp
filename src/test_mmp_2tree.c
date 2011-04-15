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
#include <stdio.h>
#include <stdlib.h>
#include "mmp_2tree.h"

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

int main(void)
{
    t_mmp_2tree_s *tree;
    struct test_result_s ts;
    if ((tree = build_test_tree())==NULL)
        return EXIT_FAILURE;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_preorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "FBADCEGIH")!=0)
        return EXIT_FAILURE;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_inorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "ABCDEFGHI")!=0)
        return EXIT_FAILURE;
    memset(ts.buf, 0, sizeof(ts.buf)/sizeof(ts.buf[0])); ts.buf_i = 0;
    mmp_2tree_visit_postorder(tree, visit_v, &ts);
    if (strcmp(ts.buf, "ACEDBHIGF")!=0)
        return EXIT_FAILURE;
    mmp_2tree_destroy_withdata(&tree, free_el);
    if (tree!=NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

