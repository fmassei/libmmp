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
#include "mmp_string.h"

static int test_xstrdup(void)
{
    char *p;
    if ((p = xstrdup("test"))==NULL)
        return -1;
    if (strcmp(p, "test")!=0) {
        xfree(p);
        return -1;
    }
    xfree(p);
    return 0;
}
static int test_xstrdupn(void)
{
    char *p;
    if ((p = xstrdupn("test_and_test", 4))==NULL)
        return -1;
    if (strcmp(p, "test")!=0) {
        xfree(p);
        return -1;
    }
    xfree(p);
    return 0;
}
static int test_xindex(void)
{
    char *p = "pippo";
    if (xindex(p, 'i')==p+1)
        return 0;
    return -1;
}
static int test_xrindex(void)
{
    char *p = "pippo";
    if (xrindex(p, 'p')==p+3)
        return 0;
    return -1;
}
static int test_xstrncasecmp(void)
{
    char *s0 = "test";
    char *s1 = "test2";
    char *s2 = "Test";
    if (xstrncasecmp(s0, s1, strlen(s0))!=0)
        return -1;
    if (xstrncasecmp(s1, s2, strlen(s1))==0)
        return -1;
    if (xstrncasecmp(s0, s2, strlen(s0))!=0)
        return -1;
    return 0;
}
/* taken from strtok_r man page */
static int test_xstrtok_r(void)
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
                return -1;
            if (strcmp(mix, res[i++])!=0)
                return -1;
        }
    }
    return 0;
}
static int test_str_is_trimmable(void)
{
    char p[0xff] = {0};
    int i;
    p[' '] = p['\t'] = p['\n'] = p['\r'] = 1;
    for (i=0; i<0xff; ++i)
        if (mmp_str_is_trimmable((char)i)!=p[i])
            return -1;
    return 0;
}
static int test_str_trim(void)
{
    char *p;
    if ((p = xstrdup(" test \n  \t\n"))==NULL)
        return -1;
    if (strcmp(mmp_str_trim(p), "test")!=0) {
        xfree(p);
        return -1;
    }
    xfree(p);
    return 0;
}
static int test_toSysPath(void)
{
    char *w;
    if ((w = mmp_str_pathUnixToSys("pub/beer/more"))==NULL)
        return -1;
#ifdef _WIN32
    if (strcmp(w, "pub\\\\beer\\\\more")) {
#else
    if (strcmp(w, "pub/beer/more")) {
#endif
        xfree(w);
        return -1;
    }
    xfree(w);
    if ((w = mmp_str_pathUnixToSys("/home/twinky/stuff"))==NULL)
        return -1;
#ifdef _WIN32
    if (strcmp(w, "C:\\\\home\\\\twinky\\\\stuff")) {
#else
    if (strcmp(w, "/home/twinky/stuff")) {
#endif
        xfree(w);
        return -1;
    }
    xfree(w);
    return 0;
}

int main(void)
{
    if (    test_xstrdup()!=0 ||
            test_xstrdupn()!=0 ||
            test_xrindex()!=0 ||
            test_xindex()!=0 ||
            test_xstrncasecmp()!=0 ||
            test_xstrtok_r()!=0 ||
            test_str_is_trimmable()!=0 ||
            test_str_trim()!=0 ||
            test_toSysPath()!=0 )
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

