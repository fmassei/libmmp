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
#include "mmp_date.h"
#include "mmp_memory.h"

static struct tm *get_test_tm(void)
{
    struct tm *ret;
    MMP_XMALLOC_OR_RETURN(ret, NULL);
    ret->tm_sec = 0;
    ret->tm_min = 1;
    ret->tm_hour = 2;
    ret->tm_mday = 10;
    ret->tm_mon = 6;
    ret->tm_year = 110;
    ret->tm_isdst = 0;
    return ret;
}
static int test_1123(void)
{
    char buf[100];
    char *tz;
    struct tm *mtm;
    time_t mtt;
    tz = mmp_get_tm_zone();
    mmp_set_tm_zone("GMT");
    mtm = get_test_tm();
    mtt = mktime(mtm);
    xfree(mtm);
    mmp_set_tm_zone(tz);
    mmp_time_1123_format(mtt, buf, sizeof(buf));
    if (!strcmp(buf, "Sat, 10 Jul 2010 02:01:00 GMT"))
        return 0;
    return -1;
}
static int test_1036(void)
{
    char buf[100];
    char *tz;
    struct tm *mtm;
    time_t mtt;
    tz = mmp_get_tm_zone();
    mmp_set_tm_zone("GMT");
    mtm = get_test_tm();
    mtt = mktime(mtm);
    xfree(mtm);
    mmp_time_1036_format(mtt, buf, sizeof(buf));
    mmp_set_tm_zone(tz);
    if (!strcmp(buf, "Saturday, 10-Jul-10 02:01:00 GMT"))
        return 0;
    return -1;
}
static int test_asctime(void)
{
    char buf[100];
    char *tz;
    struct tm *mtm;
    time_t mtt;
    tz = mmp_get_tm_zone();
    mmp_set_tm_zone("GMT");
    mtm = get_test_tm();
    mtt = mktime(mtm);
    xfree(mtm);
    mmp_set_tm_zone(tz);
    mmp_time_asctime_format(mtt, buf, sizeof(buf));
    if (!strcmp(buf, "Sat Jul 10 02:01:00 2010"))
        return 0;
    return -1;
}
static int test_parse(void)
{
    return 0;
}
int main(void)
{
    if (    test_1123()!=0 ||
            test_1036()!=0 ||
            test_asctime()!=0 ||
            test_parse()!=0 ) {
        mmp_trace_print(stdout);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

