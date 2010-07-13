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

/** \test mmp_string_unittest */
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

/** \test mmp_string_unittest */
char *xindex(const char *str, char c)
{
    if (str==NULL || *str=='\0') return NULL;
    while(*str!=c && *str!='\0') ++str;
    if (*str==c) return (char*)str;
    return NULL;
}

/** \test mmp_string_unittest */
int xstrncasecmp(const char *s1, const char *s2, size_t n)
{
#ifdef _WIN32
    return _strnicmp(s1, s2, n);
#else
    return strncasecmp(s1, s2, n);
#endif
}

/** \test mmp_string_unittest */
char *xstrtok_r(char *str, const char *delim, char **ctx)
{
#ifdef _WIN32
    return strtok_s(str, delim, ctx);
#else
    return strtok_r(str, delim, ctx);
#endif
}

/** \test mmp_string_unittest */
int mmp_str_is_trimmable(char c)
{
    return (c==' ' || c=='\t' || c=='\n' || c=='\r');
}

/** \test mmp_string_unittest */
char *mmp_str_ltrim(const char * str)
{
    if (str==NULL) return NULL;
    if (*str=='\0') return (char*)str;
    while (mmp_str_is_trimmable(*str) && *str!='\0') ++str;
    return (char*)str;
}

/** \test mmp_string_unittest */
char *mmp_str_rtrim(char *str)
{
    int i;
    if (str==NULL) return NULL;
    if (*str=='\0') return (char*)str;
    for (i=strlen(str)-1; mmp_str_is_trimmable(str[i]) && (i>=0); --i)
        str[i] = '\0';
    return (char*)str;
}

/** \test mmp_string_unittest */
char *mmp_str_trim(char *str)
{
    return mmp_str_rtrim(mmp_str_ltrim(str));
}

#ifdef UNIT_TESTING
static t_mmp_tap_result_e test_xstrdup(void)
{
    char *p;
    if ((p = xstrdup("test"))==NULL)
        return MMP_TAP_FAILED;
    if (strcmp(p, "test")!=0) {
        xfree(p);
        return MMP_TAP_FAILED;
    }
    xfree(p);
    return MMP_TAP_PASSED;
}
static t_mmp_tap_result_e test_xindex(void)
{
    char *p = "pippo";
    if (xindex(p, 'i')==p+1)
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
static t_mmp_tap_result_e test_xstrncasecmp(void)
{
    char *s0 = "test";
    char *s1 = "test2";
    char *s2 = "Test";
    if (xstrncasecmp(s0, s1, strlen(s0))!=0)
        return MMP_TAP_FAILED;
    if (xstrncasecmp(s1, s2, strlen(s1))==0)
        return MMP_TAP_FAILED;
    if (xstrncasecmp(s0, s2, strlen(s0))!=0)
        return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
/* taken from strtok_r man page */
static t_mmp_tap_result_e test_xstrtok_r(void)
{
    char test[80], blah[80], mix[80];
    char *sep = "\\/:;=-";
    char *word, *phrase, *brkt, *brkb;
    char *res[] = { "This:blah", "This:blat", "This:blab", "This:blag",
            "is.a:blah", "is.a:blat", "is.a:blab", "is.a:blag", "test:blah",
            "test:blat", "test:blab", "test:blag", "of:blah", "of:blat",
            "of:blab", "of:blag", "the:blah", "the:blat", "the:blab",
            "the:blag", "string:blah", "string:blat", "string:blab",
            "string:blag", "tokenizer:blah", "tokenizer:blat",
            "tokenizer:blab", "tokenizer:blag", "function.:blah",
            "function.:blat", "function.:blab", "function.:blag" };
    int i = 0;
    strcpy(test, "This;is.a:test:of=the/string\\tokenizer-function.");
    for (   word = xstrtok_r(test, sep, &brkt);
            word;
            word = xstrtok_r(NULL, sep, &brkt)) {
        strcpy(blah, "blah:blat:blab:blag");
        for (   phrase = xstrtok_r(blah, sep, &brkb);
                phrase;
                phrase = xstrtok_r(NULL, sep, &brkb)) {
            sprintf(mix, "%s:%s", word, phrase);
            if (i>=(sizeof(res)/sizeof(res[0])))
                return MMP_TAP_FAILED;
            if (strcmp(mix, res[i++])!=0)
                return MMP_TAP_FAILED;
        }
    }
    return MMP_TAP_PASSED;
}
static t_mmp_tap_result_e test_str_is_trimmable(void)
{
    char p[0xff] = {0};
    int i;
    p[' '] = p['\t'] = p['\n'] = p['\r'] = 1;
    for (i=0; i<0xff; ++i)
        if (mmp_str_is_trimmable((char)i)!=p[i])
            return MMP_TAP_FAILED;
    return MMP_TAP_PASSED;
}
static t_mmp_tap_result_e test_str_trim(void)
{
    char *p;
    if ((p = xstrdup(" test \n  \t\n"))==NULL)
        return MMP_TAP_FAILED;
    if (strcmp(mmp_str_trim(p), "test")!=0) {
        xfree(p);
        return MMP_TAP_FAILED;
    }
    xfree(p);
    return MMP_TAP_PASSED;
}
ret_t mmp_string_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret=mmp_tap_test(cycle, "xstrdup", NULL, test_xstrdup()))!=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "xindex", NULL, test_xindex()))!=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "xstrncasecmp", NULL, test_xstrncasecmp()))
                                                                !=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "xstrtok", NULL, test_xstrtok_r()))
                                                                !=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "str_is_trimmable", NULL,
                                        test_str_is_trimmable()))!=MMP_ERR_OK)
        return ret;
    if ((ret=mmp_tap_test(cycle, "str_trim", NULL,
                                        test_str_trim()))!=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */

