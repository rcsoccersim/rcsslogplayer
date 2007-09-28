dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(rcgparserexample, 0.1.0)
AC_CONFIG_SRCDIR([src/rcgparserexample.cpp])
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE([gnu])

dnl Checks for programs.
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC

AC_C_CONST

AC_LIB_RCGPARSER([LIBS="$LIBS -lrcssrcgparser"],
                 AC_MSG_ERROR([
The rcssrcgparser library cannot be found.  Please make sure it is
installed or specify it's location using CPPFLAGS and LDFLAGS.
For example

  ./configure CPPFLAGS="-I<HEADERS_DIR>" LDFLAGS="-L<LIB_DIR>"

Where HEADERS_DIR is the directory containing the parser header files
and LIB_DIR is the directory containing the parser library.
]))

AC_CHECK_HEADERS([netinet/in.h], [], [AC_MSG_ERROR([
The netinet/in.h header could not be found on your system.  Please
make sure it is installed.
])])

AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
