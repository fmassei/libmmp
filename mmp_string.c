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
#include "mmp_string.h"

char *xstrdup(const char *s)
{
    char *ret;
    if (s==NULL) return NULL;
    if ((ret = xmalloc(strlen(s) + 1))==NULL) {
        errno = ENOMEM;
        return NULL;
    }
    strcpy(ret, s);
    return ret;
}

char *xindex(char *str, char c)
{
    if (str==NULL || *str=='\0') return NULL;
    while(*str!=c && *str!='\0') ++str;
    if (*str==c) return str;
    return NULL;
}

int mmp_str_is_trimmable(char c)
{
    return (c==' ' || c=='\t' || c=='\n' || c=='\r');
}

char *mmp_str_ltrim(char *str)
{
    if (str==NULL) return NULL;
    if (*str=='\0') return str;
    while (mmp_str_is_trimmable(*str) && *str!='\0') ++str;
    return str;
}

char *mmp_str_rtrim(char *str)
{
    int i;
    if (str==NULL) return NULL;
    if (*str=='\0') return str;
    for (i=strlen(str)-1; mmp_str_is_trimmable(str[i]) && (i>=0); --i)
        str[i] = '\0';
    return str;
}

char *mmp_str_trim(char *str)
{
    return mmp_str_rtrim(mmp_str_ltrim(str));
}

#ifdef UNIT_TESTING
static enum mmp_tap_result_e test_xstrdup(void)
{
    return MMP_TAP_UNTESTED;
}
static enum mmp_tap_result_e test_xindex(void)
{
    char *p = "pippo";
    if (xindex(p, 'i')==p+1)
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
ret_t mmp_string_unittest(struct mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret=mmp_tap_test(cycle, "xstrdup", NULL, test_xstrdup()))!=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "xindex", NULL, test_xindex()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

