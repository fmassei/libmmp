#include "mmp_table.h"

/** \test test_table */
void mmp_table_destroy(t_mmp_table_s **table)
{
    if (table==NULL || *table==NULL) return;
    if ((*table)->data!=NULL)
        xfree((*table)->data);
    MMP_XFREE_AND_NULL(*table);
}

/** \todo missing unittest */
void mmp_table_destroy_withdata(t_mmp_table_s **table, void(*cback)(void**))
{
    int i;
    if (table==NULL || *table==NULL) return;
    for (i=0; i<(*table)->n_data; ++i)
        if (cback!=NULL)
            cback(&(*table)->data[i]);
    mmp_table_destroy(table);
}

/** \test test_table */
t_mmp_table_s *mmp_table_create(int size)
{
    t_mmp_table_s *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    if ((ret->data = xmalloc(sizeof(*ret->data)*size))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        xfree(ret);
        return NULL;
    }
    ret->n_alloc = size;
    ret->n_data = 0;
    return ret;
}

/** \test test_table */
t_mmp_table_id mmp_table_add(t_mmp_table_s *table, void *elem)
{
    MMP_CHECK_OR_RETURN((table!=NULL && table->data!=NULL),
            MMP_TABLE_INVALID_ID);
    if (table->n_data>=table->n_alloc) {
        void **tmp;
        tmp = xrealloc(table->data, sizeof(*table->data)*(table->n_data+1));
        if (tmp==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_TABLE_INVALID_ID;
        }
        table->data = tmp;
        ++table->n_alloc;
        return MMP_ERR_FULL;
    }
    table->data[table->n_data] = elem;
    ++table->n_data;
    return table->n_data-1;
}

/** \test test_table */
void *mmp_table_del(t_mmp_table_s *table, t_mmp_table_id id)
{
    void *ret;
    t_mmp_table_id i;
    MMP_CHECK_OR_RETURN((table!=NULL && table->data!=NULL && id>=0 &&
                        table->n_data>id), NULL);
    ret = table->data[id];
    for (i=id+1; i<table->n_data; ++i)
        table->data[i-1] = table->data[i];
    --table->n_data;
    return ret;
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_table(void)
{
    t_mmp_table_s *table;
    int i = 0, *dt;
    if ((table = mmp_table_create(10))==NULL)
        return MMP_TAP_FAILED;
    for (i=0; i<10; ++i) {
        if ((dt = xmalloc(sizeof(*dt)))==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_TAP_FAILED;
        }
        *dt = i;
        if (mmp_table_add(table, dt)!=i)
            return MMP_TAP_FAILED;
    }
    for (i=0; i<10; ++i) {
        dt = mmp_table_del(table, 0);
        if (*dt!=i)
            return MMP_TAP_FAILED;
        xfree(dt);
    }
    mmp_table_destroy(&table);
    if (table!=NULL)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}

/* do the tests */
ret_t mmp_table_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if (
            ((ret=mmp_tap_test(cycle, "mmp_table functions", NULL,
                                        test_table()))!=MMP_ERR_OK)

       )
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

