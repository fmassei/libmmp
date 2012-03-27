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
#include "mmp_vect.h"

/** \test test_vect */
void mmp_vect_destroy(t_mmp_vect_s **vect)
{
    if (vect==NULL || *vect==NULL) return;
    if ((*vect)->data!=NULL)
        xfree((*vect)->data);
    MMP_XFREE_AND_NULL(*vect);
}

/** \todo missing unittest */
void mmp_vect_destroy_withdata(t_mmp_vect_s **vect, void(*cback)(void**))
{
    int i;
    if (vect==NULL || *vect==NULL) return;
    for (i=0; i<(*vect)->n_data; ++i)
        if (cback!=NULL)
            cback(&(*vect)->data[i]);
    mmp_vect_destroy(vect);
}

/** \test test_vect */
t_mmp_vect_s *mmp_vect_create(int size)
{
    t_mmp_vect_s *ret;
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

/** \test test_vect */
t_mmp_vect_id mmp_vect_add(t_mmp_vect_s *vect, void *elem)
{
    MMP_CHECK_OR_RETURN((vect!=NULL && vect->data!=NULL),
            MMP_VECT_INVALID_ID);
    if (vect->n_data>=vect->n_alloc) {
        void **tmp;
        tmp = xrealloc(vect->data, sizeof(*vect->data)*(vect->n_data+1));
        if (tmp==NULL) {
            mmp_setError(MMP_ERR_ENOMEM);
            return MMP_VECT_INVALID_ID;
        }
        vect->data = tmp;
        ++vect->n_alloc;
    }
    vect->data[vect->n_data] = elem;
    ++vect->n_data;
    return vect->n_data-1;
}

/** \test test_vect */
void *mmp_vect_del(t_mmp_vect_s *vect, t_mmp_vect_id id)
{
    void *ret;
    t_mmp_vect_id i;
    MMP_CHECK_OR_RETURN((vect!=NULL && vect->data!=NULL && id>=0 &&
                        vect->n_data>id), NULL);
    ret = vect->data[id];
    for (i=id+1; i<vect->n_data; ++i)
        vect->data[i-1] = vect->data[i];
    --vect->n_data;
    return ret;
}

