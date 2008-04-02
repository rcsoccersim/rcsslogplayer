#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT([RCSSLogPlayer], 12.0.0, sserver-admin@users.sf.net, rcsslogplayer)
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADERS([config.h])

#AM_INIT_AUTOMAKE([gnu 1.7.2 check-news dist-bzip2 dist-zip])
AM_INIT_AUTOMAKE([gnu 1.7.2 check-news])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_LANG([C++])

AC_DISABLE_STATIC

# Checks for libraries.
AC_CHECK_LIB([m], [cos],
             [LIBS="-lm $LIBS"],
             [AC_MSG_ERROR([*** -lm not found! ***])])
AC_CHECK_LIB([z], [deflate])

# Checks for header files.
AC_CHECK_HEADERS([netinet/in.h sys/time.h unistd.h])


# To avoid WARNING, put AC_PROG_LIBTOOL after AC_CHECK_HEADERS.
AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
#AC_TYPE_INT16_T
#AC_TYPE_INT32_T
#AC_TYPE_UINT16_T
AC_TYPE_SIZE_T
AC_HEADER_TIME

# Checks for library functions.
AC_HEADER_STDC
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([memset rint select socket strerror])

# ----------------------------------------------------------
# check boost

AX_BOOST_BASE([1.32.0])
AX_BOOST_PROGRAM_OPTIONS
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"
LDFLAGS="$LDFLAGS $BOOST_LDFLAGS"

# ----------------------------------------------------------
# check rcssbase

AC_ARG_VAR(RCSSBASE, [The prefix that was used when installing rcssbase])
if test "$RCSSBASE"; then
  if test -z "$PATH"; then
    PATH="$RCSSBASE/bin"
  else
    PATH="$PATH:$RCSSBASE/bin"
  fi
  if test -z "$LDFLAGS"; then
    LDFLAGS="-L$RCSSBASE/lib"
  else
    LDFLAGS="$LDFLAGS -L$RCSSBASE/lib"
  fi
  if test -z "$CPPFLAGS"; then
    CPPFLAGS="-I$RCSSBASE/include"
  else
    CPPFLAGS="$CPPFLAGS -I$RCSSBASE/include"
  fi
fi

AC_CHECK_LIB([rcssgz], [main],
             [],
             [AC_MSG_ERROR([Could not find the rcssbase/gz library or it is out of
                            date. Please (re)install rcssbase before proceeding, or
                            specify where it was installed with the RCSSBASE
                            environment variable.
])])

# ----------------------------------------------------------
# check Qt4
AC_ARG_ENABLE(qt4,
              AC_HELP_STRING([--enable-qt4],
                             [use Qt4 GUI tool kit (default yes)]))
if test "x$enableval" != "xno"; then
  AC_MSG_NOTICE(enabled Qt4)

  AX_QT4(4.3.0)

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
              AC_HELP_STRING([--enable-classic],
                             [build classic logplayer (default yes)]))
if test "x$enableval" != "xno"; then
  AC_MSG_NOTICE(enabled classic logplayer)

  AC_SUBST(CLASSIC_CPPFLAGS)
  AC_SUBST(CLASSIC_CXXFLAGS)
  AC_SUBST(CLASSIC_LDFLAGS)
  AC_SUBST(CLASSIC_LIBS)

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
    AC_CHECK_LIB([Xext], [XmbufCreateBuffers], [CLASSIC_LIBS="$CLASSIC_LIBS -lXext"], [], [$CLASSIC_LIBS])
    AC_CHECK_LIB([Xpm], [XpmReadFileToPixmap], [CLASSIC_LIBS="$CLASSIC_LIBS -lXpm"], [], [$CLASSIC_LIBS])
    AC_CHECK_LIB([Xaw], [XawInitializeWidgetSet], [CLASSIC_LIBS="$CLASSIC_LIBS -lXaw"], [], [$CLASSIC_LIBS])
    CPPFLAGS="$save_CPPFLAGS"
    LDFLAGS="$save_LDFLAGS"
    LIBS="$save_LIBS"
  fi

  AC_CHECK_LIB([rcssnet], [main],
               [],
               [AC_MSG_ERROR([Could not find the rcssbase/net library or it is out of
                              date. Please (re)install rcssbase before proceeding, or
                              specify where it was installed with the RCSSBASE
                              environment variable.])])
  AC_CHECK_LIB([rcssbase], [main],
               [],
               [AC_MSG_ERROR([Could not find the rcssbase library or it is out of
                              date. Please (re)install rcssbase before proceeding, or
                              specify where it was installed with the RCSSBASE
                              environment variable.])])

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
