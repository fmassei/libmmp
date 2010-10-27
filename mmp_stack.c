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
#include "mmp_stack.h"

/** \test test_stack */
void mmp_stack_destroy(t_mmp_stack_s **stack)
{
    if (stack==NULL || *stack==NULL) return;
    if ((*stack)->data!=NULL)
        xfree((*stack)->data);
    MMP_XFREE_AND_NULL(*stack);
}

/** \test test_stack */
t_mmp_stack_s *mmp_stack_create(int size)
{
    t_mmp_stack_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    if ((ret->data = xmalloc(sizeof(*ret->data)*size))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(ret);
        return NULL;
    }
    ret->max_size = size;
    ret->n_data = 0;
    return ret;
}

/** \test test_stack */
ret_t mmp_stack_push(t_mmp_stack_s *stack, void *elem)
{
    MMP_CHECK_OR_RETURN((stack!=NULL && stack->data!=NULL), MMP_ERR_PARAMS);
    if (stack->n_data>=stack->max_size)
        return MMP_ERR_FULL;
    stack->data[stack->n_data++] = elem;
    return MMP_ERR_OK;
}

/** \test test_stack */
ret_t mmp_stack_pop(t_mmp_stack_s *stack, void **elem)
{
    MMP_CHECK_OR_RETURN((stack!=NULL && stack->data!=NULL && elem!=NULL),
                        MMP_ERR_PARAMS);
    if (stack->n_data<=0)
        return MMP_ERR_EMPTY;
    *elem = stack->data[--stack->n_data];
    return MMP_ERR_OK;
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_stack(void)
{
    t_mmp_stack_s *stack;
    ret_t res;
    int i = 0, *dt;
    if ((stack = mmp_stack_create(10))==NULL)
        return MMP_TAP_FAILED;
    for (i=1; ; ++i) {
        if ((dt = xmalloc(sizeof(*dt)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_TAP_FAILED;
        }
        *dt = i;
        res = mmp_stack_push(stack, dt);
        if (res==MMP_ERR_FULL) break;
        else if (res==MMP_ERR_OK) continue;
        else return MMP_TAP_FAILED;
    }
    for (i=10; i>0; --i) {
        res = mmp_stack_pop(stack, &dt);
        if (*((int*)dt)!=i)
            return MMP_TAP_FAILED;
        xfree(dt);
    }
    mmp_stack_destroy(&stack);
    if (stack!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

/* do the tests */
ret_t mmp_stack_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp_stack functions", NULL,
                                        test_stack()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

