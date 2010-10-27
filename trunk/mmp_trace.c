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
#include "mmp_trace.h"

#define MMP_TRACE_FILENAME_LEN  0xff
#define MMP_TRACE_EXTRA_LEN     0xff
#define MMP_TRACE_ARR_LEN       20

typedef struct mmp_trace_s {
    char    filename[MMP_TRACE_FILENAME_LEN];
    int     line;
    ret_t   ret;
    err_t   err;
    char    extramsg[MMP_TRACE_EXTRA_LEN];
} t_mmp_trace_s;

static t_mmp_trace_s m_trace_arr[MMP_TRACE_ARR_LEN];
static int m_trace_num = 0;

/** \todo missing unittest */
void mmp_trace_create(const char * __restrict filename, int line, ret_t ret,
                                err_t err, const char * __restrict extramsg)
{
    if (m_trace_num>=MMP_TRACE_ARR_LEN)
        return;
    if (filename!=NULL) {
        xstrncpy(m_trace_arr[m_trace_num].filename, filename,
                                                    MMP_TRACE_FILENAME_LEN-1);
        m_trace_arr[m_trace_num].filename[MMP_TRACE_FILENAME_LEN-1] = '\0';
    } else {
        m_trace_arr[m_trace_num].filename[0] = '\0';
    }
    m_trace_arr[m_trace_num].line = line;
    m_trace_arr[m_trace_num].ret = ret;
    m_trace_arr[m_trace_num].err = err;
    if (extramsg!=NULL) {
        xstrncpy(m_trace_arr[m_trace_num].extramsg, extramsg,
                                                    MMP_TRACE_EXTRA_LEN-1);
        m_trace_arr[m_trace_num].extramsg[MMP_TRACE_EXTRA_LEN-1] = '\0';
    } else {
        m_trace_arr[m_trace_num].extramsg[0] = '\0';
    }
    m_trace_num++;
}

/** \todo missing unittest */
void mmp_trace_print(FILE *out)
{
    int i;
    for (i=0; i<m_trace_num; ++i) {
        fprintf(out, "%d) %s:%d\n", 
                        i, m_trace_arr[i].filename, m_trace_arr[i].line);
        if (m_trace_arr[i].ret!=MMP_ERR_OK)
            fprintf(out, "\tintern(%d)\t%s\n",
                        m_trace_arr[i].ret,
                        mmp_error_ret_getdesc(m_trace_arr[i].ret));
        if (m_trace_arr[i].err!=0)
            fprintf(out, "\tsystem(%d)\t%s\n",
                        m_trace_arr[i].err,
                        mmp_error_err_getdesc(m_trace_arr[i].err));
        if (m_trace_arr[i].extramsg[0]!='\0')
            fprintf(out, "\textra:\t%s\n",
                        m_trace_arr[i].extramsg);
        fprintf(out, "--- --- ---\n");
    }
}

/** \todo missing unittest */
void mmp_trace_reset(void)
{
    m_trace_num = 0;
}

