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
#include "mmp_tap.h"

static void freetest(t_mmp_tap_entry_s **entry)
{
    if (entry==NULL || *entry==NULL) return;
    if ((*entry)->desc) xfree((*entry)->desc);
    if ((*entry)->comment) xfree((*entry)->comment);
    xfree(*entry);
    *entry = NULL;
}
static void freetestv(void **ptr)
{
    freetest((t_mmp_tap_entry_s**)ptr);
}

void mmp_tap_freecycle(t_mmp_tap_cycle_s **cycle)
{
    if (cycle==NULL || *cycle==NULL) return;
    if ((*cycle)->name) xfree((*cycle)->name);
    if ((*cycle)->tests)
        mmp_list_delete_withdata(&((*cycle)->tests), freetestv);
    xfree(*cycle);
    *cycle = NULL;
}

t_mmp_tap_cycle_s *mmp_tap_startcycle(const char *name)
{
    t_mmp_tap_cycle_s *ret = NULL;
    if (    ((ret = xmalloc(sizeof(*ret)))==NULL) ||
            ((ret->name = xstrdup(name))==NULL) ||
            ((ret->tests = mmp_list_create())==NULL)    ) {
        mmp_setError(MMP_ERR_ENOMEM);
        mmp_tap_freecycle(&ret);
        return NULL;
    }
    return ret;
}

static t_mmp_tap_entry_s *create_test(const char * __restrict desc,
                    const char * __restrict comment, t_mmp_tap_result_e res)
{
    t_mmp_tap_entry_s *ret = NULL;
    if (    ((ret = xmalloc(sizeof(*ret)))==NULL)) {
        mmp_setError(MMP_ERR_ENOMEM);
        freetest(&ret);
        return NULL;
    }
    ret->desc = ret->comment = NULL;
    if (    ((desc!=NULL) && ((ret->desc = xstrdup(desc))==NULL)) ||
            ((comment!=NULL) && ((ret->comment = xstrdup(comment))==NULL))  ) {
        mmp_setError(MMP_ERR_ENOMEM);
        freetest(&ret);
        return NULL;
    }
    ret->result = res;
    return ret;
}

ret_t mmp_tap_test(t_mmp_tap_cycle_s * __restrict cycle,
                    const char * __restrict desc,
                    const char * __restrict comment, t_mmp_tap_result_e res)
{
    t_mmp_tap_entry_s * __restrict entry;
    if (cycle==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    if ((entry = create_test(desc, comment, res))==NULL) {
        mmp_setError(MMP_ERR_GENERIC);
        return MMP_ERR_GENERIC;
    }
    return mmp_list_add_data(cycle->tests, entry);
}

ret_t mmp_tap_test_passed(t_mmp_tap_cycle_s * __restrict cycle, 
                    const char * __restrict desc,
                    const char * __restrict comment)
{
    return mmp_tap_test(cycle, desc, comment, MMP_TAP_PASSED);
}

ret_t mmp_tap_test_failed(t_mmp_tap_cycle_s * __restrict cycle, 
                    const char * __restrict desc,
                    const char * __restrict comment)
{
    return mmp_tap_test(cycle, desc, comment, MMP_TAP_FAILED);
}

ret_t mmp_tap_test_skipped(t_mmp_tap_cycle_s * __restrict cycle, 
                    const char * __restrict desc,
                    const char * __restrict comment)
{
    return mmp_tap_test(cycle, desc, comment, MMP_TAP_UNTESTED);
}

ret_t mmp_tap_print(t_mmp_tap_cycle_s *cycle, FILE *out)
{
    t_mmp_listelem_s *el;
    t_mmp_tap_entry_s *entry;
    int i;
    if (cycle==NULL || out==NULL) {
        mmp_setError(MMP_ERR_PARAMS);
        return MMP_ERR_PARAMS;
    }
    fprintf(out, "1..%d\n", cycle->tests->nelems);
    i = 1;
    for (el = cycle->tests->head; el!=NULL; el=el->next) {
        entry = (t_mmp_tap_entry_s *)(el->data);
        if (entry->comment!=NULL)
            fprintf(out, "#%s\n", entry->comment);
        if (entry->result==MMP_TAP_PASSED)
            fprintf(out, "ok %d", i);
        else if (entry->result==MMP_TAP_FAILED)
            fprintf(out, "not ok %d", i);
        else
            fprintf(out, "ok %d #skipped", i);
        if (entry->desc!=NULL)
            fprintf(out, " - %s", entry->desc);
        fprintf(out, "\n");
        ++i;
    }
    return MMP_ERR_OK;
}

