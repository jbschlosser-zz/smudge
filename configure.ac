AC_INIT([smudge], [0.0.1], [jbschlosser@gmail.com])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_PROG_CC
AC_PROG_RANLIB
AM_PROG_AR
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile tests/Makefile])
AC_ARG_WITH([tests], AS_HELP_STRING([--without-tests], [Build without unit tests]))
AS_IF([test "x$with_tests" != "xno"], [
    PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])
])
AM_CONDITIONAL([TESTS_ENABLED], [test "x$with_tests" != "xno"])
PKG_CHECK_MODULES([GUILE], [guile-2.0])
PKG_CHECK_MODULES([PCRE], [libpcre])
AC_OUTPUT
