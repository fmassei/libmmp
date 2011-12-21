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
#include "mmp_string.h"

/** \test test_mmp_string */
char *xstrdup(const char *s)
{
    char * __restrict ret;
    if (s==NULL) return NULL;
    if ((ret = xmalloc(strlen(s) + 1))==NULL) {
        errno = ENOMEM;
        return NULL;
    }
    strcpy(ret, s);
    return ret;
}

/** \todo missing unittest */
wchar_t *xwcsdup(const wchar_t *s)
{
    wchar_t * __restrict ret;
    if (s==NULL) return NULL;
    if ((ret = xmalloc((wcslen(s) + 1)*sizeof(*ret)))==NULL) {
        errno = ENOMEM;
        return NULL;
    }
    wcscpy(ret, s);
    return ret;
}

/** \test test_mmp_string */
char *xstrdupn(const char *s, size_t n)
{
    char * __restrict ret;
    size_t tc;
    if (s==NULL) return NULL;
    tc = (n<=strlen(s)) ? n : strlen(s);
    if ((ret = xmalloc(tc + 1))==NULL) {
        errno = ENOMEM;
        return NULL;
    }
    if (tc>0)
        memcpy(ret, s, tc);
    ret[tc] = '\0';
    return ret;
}

/** \test test_mmp_string */
char *xindex(const char *str, char c)
{
    if (str==NULL || *str=='\0') return NULL;
    while(*str!=c && *str!='\0') ++str;
    if (*str==c) return (char*)str;
    return NULL;
}

/** \test test_mmp_string */
char *xrindex(const char *str, char c)
{
    size_t l;
    if (str==NULL || *str=='\0') return NULL;
    l = strlen(str);
    while(str[l]!=c && l>=0) --l;
    if (str[l]==c) return (char*)(str+l);
    return NULL;
}

/** \test test_mmp_string */
int xstrncasecmp(const char *s1, const char *s2, size_t n)
{
#ifdef _WIN32
    return _strnicmp(s1, s2, n);
#else
    return strncasecmp(s1, s2, n);
#endif
}

/** \test test_mmp_string */
char *xstrtok_r(char *str, const char *delim, char **ctx)
{
#ifdef _WIN32
    return strtok_s(str, delim, ctx);
#else
    return strtok_r(str, delim, ctx);
#endif
}

/** \test test_mmp_string */
int mmp_str_is_trimmable(char c)
{
    return (c==' ' || c=='\t' || c=='\n' || c=='\r');
}

/** \test test_mmp_string */
char *mmp_str_ltrim(const char * str)
{
    if (str==NULL) return NULL;
    if (*str=='\0') return (char*)str;
    while (mmp_str_is_trimmable(*str) && *str!='\0') ++str;
    return (char*)str;
}

/** \test test_mmp_string */
char *mmp_str_rtrim(char *str)
{
    int i;
    if (str==NULL) return NULL;
    if (*str=='\0') return (char*)str;
    for (i=strlen(str)-1; mmp_str_is_trimmable(str[i]) && (i>=0); --i)
        str[i] = '\0';
    return (char*)str;
}

/** \test test_mmp_string */
char *mmp_str_trim(char *str)
{
    return mmp_str_rtrim(mmp_str_ltrim(str));
}

/** \test test_mmp_string */
char *mmp_str_pathUnixToSys(const char *str)
{
#ifndef _WIN32
    return xstrdup(str);
#else
    char *ret;
    unsigned int i, j;
    int nsl, is_absolute;
    is_absolute = (*str=='/') ? 1 : 0;
    for (i=0, nsl=0; i<strlen(str); ++i)
        if (str[i]=='/')
            ++nsl;
    if (nsl==0)
        return xstrdup(str);
    if ((ret = xmalloc(strlen(str)+nsl+(is_absolute?2:0)+1))==NULL) {
        mmp_setError(MMP_ERR_ENOMEM);
        return NULL;
    }
    i = j = 0;
    if (is_absolute) {
        ret[j++] = 'C'; ret[j++] = ':'; ret[j++] = '\\'; ret[j++] = '\\';
        i++;
    }
    for (; i<strlen(str); ++i, ++j) {
        if (str[i]=='/') {
            ret[j] = ret[j+1] = '\\';
            ++j;
        } else {
            ret[j] = str[i];
        }
    }
    ret[j] = '\0';
    return ret;
#endif
}

