dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(rcgparserexample, 0.1.0)
AC_CONFIG_SRCDIR([config.h.in])
AM_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([gnu])

dnl Checks for programs.
AC_LANG(C++)
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
AC_LIB_RCGPARSER([LIBS="$LIBS -lrcssrcgparser"],
                 AC_MSG_ERROR([
The rcssrcgparser library cannot be found.  Please make sure it is
installed or specify it's location using CPPFLAGS and LDFLAGS.
For example

  ./configure CPPFLAGS="-I<HEADERS_DIR>" LDFLAGS="-L<LIB_DIR>"

Where HEADERS_DIR is the directory containing the parser header files
and LIB_DIR is the directory containing the parser library.
]))

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h], [], [AC_MSG_ERROR([
The netinet/in.h header could not be found on your system.  Please
make sure it is installed.
])])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

# Checks for library functions.

AC_CONFIG_FILES([Makefile
                 src/Makefile])
AC_OUTPUT
