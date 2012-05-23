#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([rcsslogplayer],[15.1.0],[sserver-admin@users.sf.net],[rcsslogplayer])

#LT_PREREQ(2.2.4)

#AM_INIT_AUTOMAKE([gnu 1.7.2 check-news dist-bzip2 dist-zip])
AM_INIT_AUTOMAKE([gnu 1.7.2 check-news])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

##################################################
# Checks for programs.
##################################################

AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

##################################################
# Checks for libraries.
##################################################

AC_CHECK_LIB([m], [cos],
             [LIBS="-lm $LIBS"],
             [AC_MSG_ERROR([*** -lm not found! ***])])
AC_CHECK_LIB([z], [deflate])

##################################################
# Checks for header files.
##################################################

AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h])
AC_CHECK_HEADERS([netinet/in.h sys/time.h unistd.h])

##################################################
# libtool settings
##################################################

# To avoid WARNING, put LT_INIT after AC_CHECK_HEADERS.
#LT_INIT([shared disable-static])
#LT_LANG([C++])
AC_ENABLE_SHARED
AC_DISABLE_STATIC
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

##################################################
# Checks for typedefs, structures, and compiler characteristics.
##################################################

AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T

AC_CHECK_TYPES([socklen_t], [], [], [
#include <sys/types.h>
#include <sys/socket.h>
])

##################################################
# Checks for library functions.
##################################################

AC_HEADER_STDC
AC_FUNC_FORK
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gethostbyname inet_ntoa memset pow])
AC_CHECK_FUNCS([pow rint select socket sqrt strerror strtol])

# ----------------------------------------------------------
# check boost

AX_BOOST_BASE([1.32.0])
AX_BOOST_PROGRAM_OPTIONS
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"

# ----------------------------------------------------------
# check Qt4
QT4MODULES="QtCore QtGui QtNetwork"

AC_ARG_ENABLE(gl,
              AC_HELP_STRING([--enable-gl],
                             [enable GLWidget for Qt4 (default no)]))
if test "x$enable_gl" == "xyes"; then
  AC_MSG_NOTICE(enabled QtOpenGL)
  CFLAGS="-DUSE_GLWIDGET $CFLAGS"
  CXXFLAGS="-DUSE_GLWIDGET $CXXFLAGS"
  QT4MODULES="$QT4MODULES QtOpenGL"
fi

AX_QT4([4.1.0],[$QT4MODULES])

if test x$have_qt4 != xyes ; then
  AC_MSG_ERROR([$QT4MODULES could not be found.])
fi

# ----------------------------------------------------------

AC_CONFIG_FILES([Makefile
                 rcsslogplayer/Makefile
                 tool/Makefile
                 qt4/Makefile])
AC_OUTPUT
