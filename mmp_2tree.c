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
void mmp_2tree_visit_preorder(t_mmp_2tree_s *tree, void(*fnc)(void*))
{
    if (tree==NULL) return;
    if (fnc!=NULL) fnc(tree->data);
    if (tree->l!=NULL) mmp_2tree_visit_preorder(tree->l, fnc);
    if (tree->r!=NULL) mmp_2tree_visit_preorder(tree->r, fnc);
}

/** \test test_2tree */
void mmp_2tree_visit_inorder(t_mmp_2tree_s *tree, void(*fnc)(void*))
{
    if (tree==NULL) return;
    if (tree->l!=NULL) mmp_2tree_visit_inorder(tree->l, fnc);
    if (fnc!=NULL) fnc(tree->data);
    if (tree->r!=NULL) mmp_2tree_visit_inorder(tree->r, fnc);
}

/** \test test_2tree */
void mmp_2tree_visit_postorder(t_mmp_2tree_s *tree, void(*fnc)(void*))
{
    if (tree==NULL) return;
    if (tree->l!=NULL) mmp_2tree_visit_postorder(tree->l, fnc);
    if (tree->r!=NULL) mmp_2tree_visit_postorder(tree->r, fnc);
    if (fnc!=NULL) fnc(tree->data);
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

static char buf[10];
static int buf_i;
static void visit(void *el)
{
    buf[buf_i++] = ((char*)el)[0];
}

static t_mmp_tap_result_e test_2tree(void)
{
    t_mmp_2tree_s *tree;
    if ((tree = build_test_tree())==NULL)
        return MMP_TAP_FAILED;
    memset(buf, 0, sizeof(buf)/sizeof(buf[0])); buf_i = 0;
    mmp_2tree_visit_preorder(tree, visit);
    if (strcmp(buf, "FBADCEGIH")!=0)
        return MMP_TAP_FAILED;
    memset(buf, 0, sizeof(buf)/sizeof(buf[0])); buf_i = 0;
    mmp_2tree_visit_inorder(tree, visit);
    if (strcmp(buf, "ABCDEFGHI")!=0)
        return MMP_TAP_FAILED;
    memset(buf, 0, sizeof(buf)/sizeof(buf[0])); buf_i = 0;
    mmp_2tree_visit_postorder(tree, visit);
    if (strcmp(buf, "ACEDBHIGF")!=0)
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
