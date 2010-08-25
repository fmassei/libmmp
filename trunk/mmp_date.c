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
#include "mmp_date.h"

#ifdef _WIN32
/* no strptime in win32 :( */
#include <ctype.h>
#include <locale.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include "mmp_string.h"
typedef struct {
    const char *abday[7];
    const char *day[7];
    const char *abmon[12];
    const char *mon[12];
    const char *am_pm[2];
    const char *d_t_fmt;
    const char *d_fmt;
    const char *t_fmt;
    const char *t_fmt_ampm;
} _TimeLocale;
static const _TimeLocale _DefaultTimeLocale = 
{
    {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"},
    {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday",
        "Saturday"},
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
        "Nov", "Dec"},
    { "January", "February", "March", "April", "May", "June", "July",
	    "August", "September", "October", "November", "December"},
    {"AM", "PM"},
    "%a %b %d %H:%M:%S %Y",
    "%m/%d/%y",
    "%H:%M:%S",
    "%I:%M:%S %p"
};
static const _TimeLocale *_CurrentTimeLocale = &_DefaultTimeLocale;
#define	_ctloc(x)		(_CurrentTimeLocale->x)

#define TM_YEAR_BASE    1900
/*
 * We do not implement alternate representations. However, we always
 * check whether a given modifier is allowed for a certain conversion.
 */
#define ALT_E			0x01
#define ALT_O			0x02
#define	LEGAL_ALT(x)		{ if (alt_format & ~(x)) return NULL; }

static char gmt[] = { "GMT" };
static char utc[] = { "UTC" };
/* RFC-822/RFC-2822 */
static const char * const nast[5] = { "EST", "CST", "MST", "PST", "\0\0\0"};
static const char * const nadt[5] = {"EDT", "CDT", "MDT", "PDT", "\0\0\0"};

static const unsigned char *conv_num(const unsigned char *buf, int *dest,
                                        unsigned int llim, unsigned int ulim)
{
	unsigned int result = 0;
	unsigned char ch;
	/* The limit also determines the number of valid digits. */
	unsigned int rulim = ulim;
	ch = *buf;
	if (ch < '0' || ch > '9')
		return NULL;
	do {
		result *= 10;
		result += ch - '0';
		rulim /= 10;
		ch = *++buf;
	} while ((result * 10 <= ulim) && rulim && ch >= '0' && ch <= '9');
	if (result < llim || result > ulim)
		return NULL;
	*dest = result;
	return buf;
}

static const unsigned char *find_string(const unsigned char *bp, int *tgt,
                                        const char * const *n1,
                                        const char * const *n2, int c)
{
	int i;
	unsigned int len;
	/* check full name - then abbreviated ones */
	for (; n1 != NULL; n1 = n2, n2 = NULL) {
		for (i = 0; i < c; i++, n1++) {
			len = strlen(*n1);
			if (xstrncasecmp(*n1, (const char *)bp, len) == 0) {
				*tgt = i;
				return bp + len;
			}
		}
	}
	/* Nothing matched */
	return NULL;
}

char *xstrptime(const char *buf, const char *fmt, struct tm *tm)
{
	unsigned char c;
	const unsigned char *bp, *ep;
	int alt_format, i, split_year = 0, neg = 0, offs;
	const char *new_fmt;

	bp = (const unsigned char *)buf;

	while (bp != NULL && (c = *fmt++) != '\0') {
		/* Clear `alternate' modifier prior to new conversion. */
		alt_format = 0;
		i = 0;

		/* Eat up white-space. */
		if (isspace(c)) {
			while (isspace(*bp))
				bp++;
			continue;
		}

		if (c != '%')
			goto literal;


again:		switch (c = *fmt++) {
		case '%':	/* "%%" is converted to "%". */
literal:
			if (c != *bp++)
				return NULL;
			LEGAL_ALT(0);
			continue;

		/*
		 * "Alternative" modifiers. Just set the appropriate flag
		 * and start over again.
		 */
		case 'E':	/* "%E?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_E;
			goto again;

		case 'O':	/* "%O?" alternative conversion modifier. */
			LEGAL_ALT(0);
			alt_format |= ALT_O;
			goto again;

		/*
		 * "Complex" conversion rules, implemented through recursion.
		 */
		case 'c':	/* Date and time, using the locale's format. */
			new_fmt = _ctloc(d_t_fmt);
			goto recurse;

		case 'D':	/* The date as "%m/%d/%y". */
			new_fmt = "%m/%d/%y";
			LEGAL_ALT(0);
			goto recurse;

		case 'F':	/* The date as "%Y-%m-%d". */
			new_fmt = "%Y-%m-%d";
			LEGAL_ALT(0);
			goto recurse;

		case 'R':	/* The time as "%H:%M". */
			new_fmt = "%H:%M";
			LEGAL_ALT(0);
			goto recurse;

		case 'r':	/* The time in 12-hour clock representation. */
			new_fmt =_ctloc(t_fmt_ampm);
			LEGAL_ALT(0);
			goto recurse;

		case 'T':	/* The time as "%H:%M:%S". */
			new_fmt = "%H:%M:%S";
			LEGAL_ALT(0);
			goto recurse;

		case 'X':	/* The time, using the locale's format. */
			new_fmt =_ctloc(t_fmt);
			goto recurse;

		case 'x':	/* The date, using the locale's format. */
			new_fmt =_ctloc(d_fmt);
		    recurse:
			bp = (const unsigned char *)xstrptime((const char *)bp,
							    new_fmt, tm);
			LEGAL_ALT(ALT_E);
			continue;

		/*
		 * "Elementary" conversion rules.
		 */
		case 'A':	/* The day of week, using the locale's form. */
		case 'a':
			bp = find_string(bp, &tm->tm_wday, _ctloc(day),
					_ctloc(abday), 7);
			LEGAL_ALT(0);
			continue;

		case 'B':	/* The month, using the locale's form. */
		case 'b':
		case 'h':
			bp = find_string(bp, &tm->tm_mon, _ctloc(mon),
					_ctloc(abmon), 12);
			LEGAL_ALT(0);
			continue;

		case 'C':	/* The century number. */
			i = 20;
			bp = conv_num(bp, &i, 0, 99);

			i = i * 100 - TM_YEAR_BASE;
			if (split_year)
				i += tm->tm_year % 100;
			split_year = 1;
			tm->tm_year = i;
			LEGAL_ALT(ALT_E);
			continue;

		case 'd':	/* The day of month. */
		case 'e':
			bp = conv_num(bp, &tm->tm_mday, 1, 31);
			LEGAL_ALT(ALT_O);
			continue;

		case 'k':	/* The hour (24-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'H':
			bp = conv_num(bp, &tm->tm_hour, 0, 23);
			LEGAL_ALT(ALT_O);
			continue;

		case 'l':	/* The hour (12-hour clock representation). */
			LEGAL_ALT(0);
			/* FALLTHROUGH */
		case 'I':
			bp = conv_num(bp, &tm->tm_hour, 1, 12);
			if (tm->tm_hour == 12)
				tm->tm_hour = 0;
			LEGAL_ALT(ALT_O);
			continue;

		case 'j':	/* The day of year. */
			i = 1;
			bp = conv_num(bp, &i, 1, 366);
			tm->tm_yday = i - 1;
			LEGAL_ALT(0);
			continue;

		case 'M':	/* The minute. */
			bp = conv_num(bp, &tm->tm_min, 0, 59);
			LEGAL_ALT(ALT_O);
			continue;

		case 'm':	/* The month. */
			i = 1;
			bp = conv_num(bp, &i, 1, 12);
			tm->tm_mon = i - 1;
			LEGAL_ALT(ALT_O);
			continue;

		case 'p':	/* The locale's equivalent of AM/PM. */
			bp = find_string(bp, &i, _ctloc(am_pm), NULL, 2);
			if (tm->tm_hour > 11)
				return NULL;
			tm->tm_hour += i * 12;
			LEGAL_ALT(0);
			continue;

		case 'S':	/* The seconds. */
			bp = conv_num(bp, &tm->tm_sec, 0, 61);
			LEGAL_ALT(ALT_O);
			continue;

#ifndef TIME_MAX
#define TIME_MAX	_I64_MAX
#endif
		case 's':	/* seconds since the epoch */
			{
				time_t sse = 0;
                unsigned __int64 rulim = TIME_MAX;

				if (*bp < '0' || *bp > '9') {
					bp = NULL;
					continue;
				}

				do {
					sse *= 10;
					sse += *bp++ - '0';
					rulim /= 10;
				} while ((sse * 10 <= TIME_MAX) &&
					 rulim && *bp >= '0' && *bp <= '9');

				if (sse < 0 || (unsigned __int64)sse > TIME_MAX) {
					bp = NULL;
					continue;
				}
                if (_localtime64_s(tm, &sse)!=0)
					bp = NULL;
			}
			continue;

		case 'U':	/* The week of year, beginning on sunday. */
		case 'W':	/* The week of year, beginning on monday. */
			/*
			 * XXX This is bogus, as we can not assume any valid
			 * information present in the tm structure at this
			 * point to calculate a real value, so just check the
			 * range for now.
			 */
			 bp = conv_num(bp, &i, 0, 53);
			 LEGAL_ALT(ALT_O);
			 continue;

		case 'w':	/* The day of week, beginning on sunday. */
			bp = conv_num(bp, &tm->tm_wday, 0, 6);
			LEGAL_ALT(ALT_O);
			continue;

		case 'u':	/* The day of week, monday = 1. */
			bp = conv_num(bp, &i, 1, 7);
			tm->tm_wday = i % 7;
			LEGAL_ALT(ALT_O);
			continue;

		case 'g':	/* The year corresponding to the ISO week
				 * number but without the century.
				 */
			bp = conv_num(bp, &i, 0, 99);
			continue;

		case 'G':	/* The year corresponding to the ISO week
				 * number with century.
				 */
			do
				bp++;
			while (isdigit(*bp));
			continue;

		case 'V':	/* The ISO 8601:1988 week number as decimal */
			bp = conv_num(bp, &i, 0, 53);
			continue;

		case 'Y':	/* The year. */
			i = TM_YEAR_BASE;	/* just for data sanity... */
			bp = conv_num(bp, &i, 0, 9999);
			tm->tm_year = i - TM_YEAR_BASE;
			LEGAL_ALT(ALT_E);
			continue;

		case 'y':	/* The year within 100 years of the epoch. */
			/* LEGAL_ALT(ALT_E | ALT_O); */
			bp = conv_num(bp, &i, 0, 99);

			if (split_year)
				/* preserve century */
				i += (tm->tm_year / 100) * 100;
			else {
				split_year = 1;
				if (i <= 68)
					i = i + 2000 - TM_YEAR_BASE;
				else
					i = i + 1900 - TM_YEAR_BASE;
			}
			tm->tm_year = i;
			continue;

		case 'Z':
			_tzset();
			if (strncmp((const char *)bp, gmt, 3) == 0) {
				tm->tm_isdst = 0;
#ifdef TM_GMTOFF
				tm->TM_GMTOFF = 0;
#endif
#ifdef TM_ZONE
				tm->TM_ZONE = gmt;
#endif
				bp += 3;
			} else {
				ep = find_string(bp, &i,
					       	 (const char * const *)tzname,
					       	  NULL, 2);
				if (ep != NULL) {
					tm->tm_isdst = i;
#ifdef TM_GMTOFF
					tm->TM_GMTOFF = -(timezone);
#endif
#ifdef TM_ZONE
					tm->TM_ZONE = tzname[i];
#endif
				}
				bp = ep;
			}
			continue;

		case 'z':
			/*
			 * We recognize all ISO 8601 formats:
			 * Z	= Zulu time/UTC
			 * [+-]hhmm
			 * [+-]hh:mm
			 * [+-]hh
			 * We recognize all RFC-822/RFC-2822 formats:
			 * UT|GMT
			 *          North American : UTC offsets
			 * E[DS]T = Eastern : -4 | -5
			 * C[DS]T = Central : -5 | -6
			 * M[DS]T = Mountain: -6 | -7
			 * P[DS]T = Pacific : -7 | -8
			 *          Military
			 * [A-IL-M] = -1 ... -9 (J not used)
			 * [N-Y]  = +1 ... +12
			 */
			while (isspace(*bp))
				bp++;

			switch (*bp++) {
			case 'G':
				if (*bp++ != 'M')
					return NULL;
				/*FALLTHROUGH*/
			case 'U':
				if (*bp++ != 'T')
					return NULL;
				/*FALLTHROUGH*/
			case 'Z':
				tm->tm_isdst = 0;
#ifdef TM_GMTOFF
				tm->TM_GMTOFF = 0;
#endif
#ifdef TM_ZONE
				tm->TM_ZONE = utc;
#endif
				continue;
			case '+':
				neg = 0;
				break;
			case '-':
				neg = 1;
				break;
			default:
				--bp;
				ep = find_string(bp, &i, nast, NULL, 4);
				if (ep != NULL) {
#ifdef TM_GMTOFF
					tm->TM_GMTOFF = -5 - i;
#endif
#ifdef TM_ZONE
					tm->TM_ZONE = __UNCONST(nast[i]);
#endif
					bp = ep;
					continue;
				}
				ep = find_string(bp, &i, nadt, NULL, 4);
				if (ep != NULL) {
					tm->tm_isdst = 1;
#ifdef TM_GMTOFF
					tm->TM_GMTOFF = -4 - i;
#endif
#ifdef TM_ZONE
					tm->TM_ZONE = __UNCONST(nadt[i]);
#endif
					bp = ep;
					continue;
				}

				if ((*bp >= 'A' && *bp <= 'I') ||
				    (*bp >= 'L' && *bp <= 'Y')) {
#ifdef TM_GMTOFF
					/* Argh! No 'J'! */
					if (*bp >= 'A' && *bp <= 'I')
						tm->TM_GMTOFF =
						    ('A' - 1) - (int)*bp;
					else if (*bp >= 'L' && *bp <= 'M')
						tm->TM_GMTOFF = 'A' - (int)*bp;
					else if (*bp >= 'N' && *bp <= 'Y')
						tm->TM_GMTOFF = (int)*bp - 'M';
#endif
#ifdef TM_ZONE
					tm->TM_ZONE = NULL; /* XXX */
#endif
					bp++;
					continue;
				}
				return NULL;
			}
			offs = 0;
			for (i = 0; i < 4; ) {
				if (isdigit(*bp)) {
					offs = offs * 10 + (*bp++ - '0');
					i++;
					continue;
				}
				if (i == 2 && *bp == ':') {
					bp++;
					continue;
				}
				break;
			}
			switch (i) {
			case 2:
				offs *= 100;
				break;
			case 4:
				i = offs % 100;
				if (i >= 60)
					return NULL;
				/* Convert minutes into decimal */
				offs = (offs / 100) * 100 + (i * 50) / 30;
				break;
			default:
				return NULL;
			}
			if (neg)
				offs = -offs;
			tm->tm_isdst = 0;	/* XXX */
#ifdef TM_GMTOFF
			tm->TM_GMTOFF = offs;
#endif
#ifdef TM_ZONE
			tm->TM_ZONE = NULL;	/* XXX */
#endif
			continue;

		/*
		 * Miscellaneous conversions.
		 */
		case 'n':	/* Any kind of white-space. */
		case 't':
			while (isspace(*bp))
				bp++;
			LEGAL_ALT(0);
			continue;


		default:	/* Unknown/unsupported conversion. */
			return NULL;
		}
	}

	return (char*)bp;
}
#else
__inline char *xstrptime(const char *buf, const char *fmt, struct tm *tm)
{
    return strptime(buf, fmt, tm);
}
#endif

/** \test mmp_date_unittest */
__inline void mmp_time_1123_format(time_t t, char * datestr, size_t strsize)
{
    if (datestr==NULL || strsize==0) return;
    strftime(datestr, strsize, "%a, %d %b %Y %H:%M:%S GMT", gmtime(&t));
}

/** \test mmp_date_unittest */
__inline void mmp_time_1036_format(time_t t, char * datestr, size_t strsize)
{
    if (datestr==NULL || strsize==0) return;
    strftime(datestr, strsize, "%A, %d-%b-%y %H:%M:%S GMT", gmtime(&t));
}

/** \test mmp_date_unittest */
__inline void mmp_time_asctime_format(time_t t, char * datestr, size_t strsize)
{
    struct tm tt;
    if (datestr==NULL || strsize==0) return;
#ifndef _WIN32
    gmtime_r(&t, &tt);
    asctime_r(&tt, datestr);
#else
    gmtime_s(&tt, &t);
    asctime_s(datestr, strsize, &tt);
#endif
    if (strlen(datestr)>0)
        datestr[strlen(datestr)-1] = '\0';
}

/* try to parse a RFC-1123 (old 822) format (Sun, 06 Nov 1994 08:49:37 GMT) */
static ret_t parse_1123_date(const char * __restrict date,
                                                        time_t * __restrict t)
{
    char * __restrict c;
    struct tm tt;
    c = xstrptime(date, "%a, %d %b %Y %H:%M:%S GMT", &tt);
    if ((c==NULL)||(*c!='\0'))
        return MMP_ERR_PARSE;
    *t = mktime(&tt);
    return MMP_ERR_OK;
}

/* try to parse a RFC-1036 (old 822) format (Sunday, 06-Nov-94 08:49:37 GMT) */
static ret_t parse_1036_date(const char * __restrict date,
                                                        time_t * __restrict t)
{
    char * __restrict c;
    struct tm tt;
    c = xstrptime(date, "%A, %d-%b-%y %H:%M:%S GMT", &tt);
    if ((c==NULL)||(*c!='\0'))
        return MMP_ERR_PARSE;
    *t = mktime(&tt);
    return MMP_ERR_OK;
}

static ret_t parse_asctime_date(const char * __restrict date,
                                                        time_t * __restrict t)
{
    char * __restrict c;
    struct tm tt;
    c = xstrptime(date, "%a %b %d %H:%M:%S %Y", &tt);
    if ((c==NULL)||(*c!='\0'))
        return MMP_ERR_PARSE;
    *t = mktime(&tt);
    return MMP_ERR_OK;
}

/* try all the parsing, returns -1 if no available parsing where possible */
/** \todo missing unittest */
ret_t mmp_parse_date(const char * __restrict date, time_t * __restrict t)
{
    if (parse_1123_date(date, t)==MMP_ERR_OK)
        return MMP_ERR_OK;
    if (parse_1036_date(date, t)==MMP_ERR_OK)
        return MMP_ERR_OK;
    if (parse_asctime_date(date, t)==MMP_ERR_OK)
        return MMP_ERR_OK;
    return MMP_ERR_PARSE;
}

/** \test mmp_date_unittest */
char *mmp_get_tm_zone(void)
{
    return getenv("TZ");
}

/** \test mmp_date_unittest */
void mmp_set_tm_zone(char *zone)
{
#ifndef _WIN32
    if (zone!=NULL)
        setenv("TZ", zone, 1);
    else
        unsetenv("TZ");
#else
    if (zone!=NULL)
        _putenv_s("TZ", zone);
    else
        _putenv_s("TZ", "");
#endif
#ifndef _WIN32
    tzset();
#else
    _tzset();
#endif
}

#ifdef UNIT_TESTING
static struct tm *get_test_tm(void)
{
    struct tm *ret;
    if ((ret = xmalloc(sizeof(*ret)))==NULL)
        return NULL;
    ret->tm_sec = 0;
    ret->tm_min = 1;
    ret->tm_hour = 2;
    ret->tm_mday = 10;
    ret->tm_mon = 6;
    ret->tm_year = 110;
    ret->tm_isdst = 0;
    return ret;
}
static enum mmp_tap_result_e test_1123(void)
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
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
static enum mmp_tap_result_e test_1036(void)
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
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
static enum mmp_tap_result_e test_asctime(void)
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
        return MMP_TAP_PASSED;
    return MMP_TAP_FAILED;
}
static enum mmp_tap_result_e test_parse(void)
{
    return MMP_TAP_UNTESTED;
}
ret_t mmp_date_unittest(t_mmp_tap_cycle_s *cycle)
{
    ret_t ret;
    if ((ret = mmp_tap_test(cycle, "date 1123", NULL, test_1123()))!=MMP_ERR_OK)
        return ret;
    if ((ret = mmp_tap_test(cycle, "date 1036", NULL, test_1036()))!=MMP_ERR_OK)
        return ret;
    if ((ret = mmp_tap_test(cycle, "date asctime", NULL, test_asctime()))
                                                                !=MMP_ERR_OK)
        return ret;
    if ((ret = mmp_tap_test(cycle, "dateparse", NULL, test_parse()))
                                                                !=MMP_ERR_OK)
        return ret;
    return MMP_ERR_OK;
}
#endif /* UNIT_TESTING */
