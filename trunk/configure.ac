AC_INIT([libmmp], [0.2], [fmassei@gmail.com])
AC_CONFIG_AUX_DIR([autotools])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_PROG_CC
LT_INIT([shared static])
AC_CONFIG_HEADERS([config.h])


###############################################################################
# debug compilation support
###############################################################################
AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug],
                              [enable debug data generation (def=no)])],
              [debugit="$enableval"],
              [debugit=no])
AC_MSG_RESULT([$debugit])
if test x"$debugit" = x"yes"; then
    AC_DEFINE([DEBUG],[],[Debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -g -Wall -Werror -O0"
    AM_CFLAGS="$AM_CFLAGS -g -Wall -Werror -O0"
else
    AC_DEFINE([NDEBUG],[],[No-debug Mode])
    AM_CXXFLAGS="$AM_CXXFLAGS -O3"
    AM_CFLAGS="$AM_CFLAGS -O3"
fi
AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_CFLAGS])

###############################################################################
# containers support
###############################################################################
AC_MSG_CHECKING([whether to build with container functions])
AC_ARG_ENABLE([containers],
              [AS_HELP_STRING([--enable-containers],
                              [enable container functions (def=yes)])],
              [containit="$enableval"],
              [containit=yes])
AC_MSG_RESULT([$containit])
AM_CONDITIONAL([WITHCONTAINERS], [test x"$containit" = x"yes"])
if test x"$containit" = x"yes"; then
    AC_DEFINE([WITHCONTAINERS],[],[Containers Mode])
fi
###############################################################################
# threads support
###############################################################################
AC_MSG_CHECKING([whether to build with container functions])
AC_ARG_ENABLE([threads],
              [AS_HELP_STRING([--enable-threads],
                              [enable threads functions (def=yes)])],
              [threadit="$enableval"],
              [threadit=yes])
AC_MSG_RESULT([$threadit])
AM_CONDITIONAL([WITHTHREADS], [test x"$threadit" = x"yes"])
if test x"$threadit" = x"yes"; then
    AC_DEFINE([WITHTHREADS],[],[Threads Mode])
fi
###############################################################################
# dynamic loading support
###############################################################################
AC_MSG_CHECKING([whether to build with dynamic loading functions])
AC_ARG_ENABLE([dl],
              [AS_HELP_STRING([--enable-dl],
                              [enable dynamic loading functions (def=yes)])],
              [dlit="$enableval"],
              [dlit=yes])
AC_MSG_RESULT([$dlit])
AM_CONDITIONAL([WITHDL], [test x"$dlit" = x"yes"])
if test x"$dlit" = x"yes"; then
    AC_DEFINE([WITHDL],[],[dl Mode])
fi
###############################################################################
# sockets support
###############################################################################
AC_MSG_CHECKING([whether to build with sockets functions])
AC_ARG_ENABLE([sockets],
              [AS_HELP_STRING([--enable-sockets],
                              [enable socket functions (def=yes)])],
              [socketit="$enableval"],
              [socketit=yes])
AC_MSG_RESULT([$socketit])
AM_CONDITIONAL([WITHSOCKETS], [test x"$socketit" = x"yes"])
if test x"$socketit" = x"yes"; then
    AC_DEFINE([WITHSOCKETS],[],[Socket Mode])
fi
###############################################################################

AC_CONFIG_FILES([
                 Makefile
                 src/Makefile
                 ])
AC_OUTPUT
