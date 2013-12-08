AC_INIT([mud], [1.0], [no@where.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
PKG_CHECK_MODULES([GUILE], [guile-2.0])
PKG_CHECK_MODULES([PCRE], [libpcre])
AC_OUTPUT
