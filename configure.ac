#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([RCSSLogPlayer],[13.1.1],[sserver-admin@users.sf.net],[rcsslogplayer])

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
LT_INIT([shared disable-static])
AC_DISABLE_STATIC
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
AC_ARG_ENABLE(qt4,
              AS_HELP_STRING([--enable-qt4],[use Qt4 GUI tool kit (default yes)]))
if test "x$enable_qt4" != "xno"; then
  AC_MSG_NOTICE(enabled Qt4)

  AX_QT4(4.1.0)

  if test x$have_qt4 != xyes ; then
    AC_MSG_NOTICE([The QtCore, QtGui or QtNetwork library could not be found.])
    AM_CONDITIONAL(BUILD_QT4, [test "1" = "0"])
  else
    AM_CONDITIONAL(BUILD_QT4, [test "1" = "1"])
  fi
else
  AM_CONDITIONAL(BUILD_QT4, [test "1" = "0"])
fi

# ----------------------------------------------------------
# check classic
AC_ARG_ENABLE(classic,
              AS_HELP_STRING([--enable-classic],[build classic logplayer (default no)]))
if test "x$enable_classic" == "xyes"; then
  AC_MSG_NOTICE(enabled classic logplayer)

  AC_SUBST(CLASSIC_CPPFLAGS)
  AC_SUBST(CLASSIC_CXXFLAGS)
  AC_SUBST(CLASSIC_LDFLAGS)
  AC_SUBST(CLASSIC_LIBS)

  # check Xaw7
#  AX_XAW7(1.0.0)

  # manual check
  if test x$have_xaw7 != xyes ; then
    AC_PATH_X
    if test "$no_x" = "yes"; then
      CLASSIC_CPPFLAGS=""
      CLASSIC_CXXFLAGS=""
      CLASSIC_LDFLAGS=""
      CLASSIC_LIBS=""
      echo "disabling classic logplayer"
      AC_DEFINE(X_DISPLAY_MISSING, 1, ["defined to 1 if the x display is diabled"])
    else
      if test "x$x_includes" != "x"; then
        CLASSIC_CPPFLAGS="$CPPFLAGS -I$x_includes"
      else
        CLASSIC_CPPFLAGS="$CPPFLAGS"
      fi

      CLASSIC_CXXFLAGS=""
      if test "x$x_libraries" != "x"; then
        CLASSIC_LDFLAGS="$LDFLAGS -L$x_libraries"
      else
        CLASSIC_LDFLAGS="$LDFLAGS"
      fi
      CLASSIC_LIBS="$X_PRE_LIBS $X_LIBS $LIBS $X_EXTRA_LIBS"

      save_CPPFLAGS="$CPPFLAGS"
      save_LDFLAGS="$LDFLAGS"
       save_LIBS="$LIBS"
      CPPFLAGS="$CLASSIC_CPPFLAGS"
      LDFLAGS="$CLASSIC_LDFLAGS"
      LIBS="$CLASSIC_LIBS"
      AC_CHECK_LIB([ICE], [IceConnectionNumber], [CLASSIC_LIBS="$CLASSIC_LIBS -lICE"], [])
      AC_CHECK_LIB([SM], [SmcSetProperties], [CLASSIC_LIBS="$CLASSIC_LIBS -lSM"], [])
      AC_CHECK_LIB([X11], [XFree], [CLASSIC_LIBS="$CLASSIC_LIBS -lX11"], [],
                   [$X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS $CLASSIC_LIBS])
      AC_CHECK_LIB([Xt], [XtAppInitialize], [CLASSIC_LIBS="$CLASSIC_LIBS -lXt"], [], [$CLASSIC_LIBS])
      AC_CHECK_LIB([Xmu], [XmuAddInitializer], [CLASSIC_LIBS="$CLASSIC_LIBS -lXmu"], [], [$CLASSIC_LIBS])
#      AC_CHECK_LIB([Xext], [XmbufCreateBuffers], [CLASSIC_LIBS="$CLASSIC_LIBS -lXext"], [], [$CLASSIC_LIBS])
#      AC_CHECK_LIB([Xpm], [XpmReadFileToPixmap], [CLASSIC_LIBS="$CLASSIC_LIBS -lXpm"], [], [$CLASSIC_LIBS])
      AC_CHECK_LIB([Xaw], [XawInitializeWidgetSet], [CLASSIC_LIBS="$CLASSIC_LIBS -lXaw"], [], [$CLASSIC_LIBS])
      CPPFLAGS="$save_CPPFLAGS"
      LDFLAGS="$save_LDFLAGS"
      LIBS="$save_LIBS"
    fi
  else
    CLASSIC_CPPFLAGS="$XAW7_CPPFLAGS"
    CLASSIC_CXXFLAGS="$XAW7_CXXFLAGS"
    CLASSIC_LDFLAGS="$XAW7_LDFLAGS"
    CLASSIC_LIBS="$XAW7_LDADD"
    AC_MSG_NOTICE([set CLASSIC_CPPFLAGS... $CLASSIC_CPPFLAGS])
    AC_MSG_NOTICE([set CLASSIC_CXXFLAGS... $CLASSIC_CXXFLAGS])
    AC_MSG_NOTICE([set CLASSIC_LDFLAGS... $CLASSIC_LDFLAGS])
    AC_MSG_NOTICE([set CLASSIC_LIBS... $CLASSIC_LIBS])
  fi
  AM_CONDITIONAL(BUILD_CLASSIC, [test "1" = "1"])
else
  AM_CONDITIONAL(BUILD_CLASSIC, [test "1" = "0"])
fi

# optional features


AC_CONFIG_FILES([Makefile
                 rcsslogplayer/Makefile
                 tool/Makefile
                 classic/Makefile
                 qt4/Makefile])
AC_OUTPUT
