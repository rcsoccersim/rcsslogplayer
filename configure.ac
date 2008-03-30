dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)

AC_INIT([RCSSLogPlayer], 12.0.0, sserver-admin@users.sf.net, rcsslogplayer)
AC_CONFIG_SRCDIR([src/types.h])
AC_CONFIG_HEADERS([config.h])

#AM_INIT_AUTOMAKE([gnu 1.7.2 check-news dist-bzip2 dist-zip])
AM_INIT_AUTOMAKE([gnu 1.7.2 check-news])

AC_PROG_MAKE_SET

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_AWK
AC_LANG(C++)

dnl Checks for libraries.

AC_CHECK_LIB([m], [cos])
AC_CHECK_LIB([z], [deflate])

dnl Checks for header files.
AC_CHECK_HEADERS([arpa/inet.h fcntl.h netdb.h netinet/in.h strings.h])
AC_CHECK_HEADERS([sys/ioctl.h sys/socket.h sys/time.h unistd.h])
AC_CHECK_HEADERS([sys/types.h])
AC_CHECK_HEADERS([sys/filio.h])

# To avoid WARNING, put AC_PROG_LIBTOOL after AC_CHECK_HEADERS.
AM_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

AC_ARG_VAR(SED, [name and location of sed executable.  If it is not
set, configure looks for sed in your PATH])

grep "SED=" ./libtool >& /dev/null
if test "$?" -ne 0; then
    AC_PATH_PROG(SED, sed)
    if test -z "$SED"; then
        AC_MSG_ERROR([
            sed cannot be found on your system.  Please install it in
            your PATH, or specify it's location to configure.  e.g.
            `./configure SED=/bin/sed`], 1)
    fi
    mv libtool libtool.tmp
    ${AWK} -v SED="$SED" '/# libtool/ \
    { if( done != "true" ) { print "SED=\"" SED "\""; done = "true"; } } \
    { print; }' libtool.tmp > libtool
    chmod +x libtool
fi

mv libtool libtool.tmp
${AWK} -v EGREP="${EGREP}" '/# libtool/ \
{ if( done != "true" ) { print "EGREP=\"" EGREP "\""; done = "true"; } } \
{ print; }' libtool.tmp > libtool
chmod +x libtool

dnl Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_HEADER_TIME

dnl Checks for library functions.
AC_HEADER_STDC
AC_PROG_GCC_TRADITIONAL
AC_FUNC_SELECT_ARGTYPES
AC_CHECK_FUNCS([gettimeofday memset select])

AX_BOOST_BASE([1.32.0])
AX_BOOST_PROGRAM_OPTIONS
CPPFLAGS="$CPPFLAGS $BOOST_CPPFLAGS"

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

AC_CHECK_LIB([rcssnet], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase/net library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcssgz], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase/conf library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])
AC_CHECK_LIB([rcssbase], [main],
             [],
             [AC_MSG_ERROR([
Could not find the rcssbase library or it is out of
date. Please (re)install rcssbase before proceeding, or
specify where it was installed with the RCSSBASE
environment variable.
])])

dnl optional features

AC_ARG_WITH(sstream,
        AC_HELP_STRING([--with-sstream],
                       [use sstream instead of strstream (default=yes)]),
                       use_sstream=$withval, use_sstream=yes)
if test "$use_sstream" = "yes"; then
   AC_CXX_HAVE_SSTREAM
fi

AC_SUBST(RCL_CPPFLAGS)
AC_SUBST(RCL_CXXFLAGS)
AC_SUBST(RCL_LDFLAGS)
AC_SUBST(RCL_LIBS)

AC_PATH_X
if test "$no_x" = "yes"; then
  RCL_CPPFLAGS=""
  RCL_CXXFLAGS=""
  RCL_LDFLAGS=""
  RCL_LIBS=""
	echo "disabling x"
	AC_DEFINE(X_DISPLAY_MISSING, 1, ["defined to 1 if the x display is diabled"])
else
  if test "x$x_includes" != "x"; then
    RCL_CPPFLAGS="$CPPFLAGS -I$x_includes"
  else
    RCL_CPPFLAGS="$CPPFLAGS"
  fi
  RCL_CXXFLAGS=""
  if test "x$x_libraries" != "x"; then
    RCL_LDFLAGS="$LDFLAGS -L$x_libraries"
  else
    RCL_LDFLAGS="$LDFLAGS"
  fi
  RCL_LIBS="$X_PRE_LIBS $X_LIBS $LIBS $X_EXTRA_LIBS"

  save_CPPFLAGS="$CPPFLAGS"
  save_LDFLAGS="$LDFLAGS"
  save_LIBS="$LIBS"
  CPPFLAGS="$RCL_CPPFLAGS"
  LDFLAGS="$RCL_LDFLAGS"
  LIBS="$RCL_LIBS"
  AC_CHECK_LIB([ICE], [IceConnectionNumber], [RCL_LIBS="$RCL_LIBS -lICE"], [])
  AC_CHECK_LIB([SM], [SmcSetProperties], [RCL_LIBS="$RCL_LIBS -lSM"], [])
  AC_CHECK_LIB([X11], [XFree], [RCL_LIBS="$RCL_LIBS -lX11"], [],
               [$X_PRE_LIBS $X_LIBS $X_EXTRA_LIBS $RCL_LIBS])
  AC_CHECK_LIB([Xt], [XtAppInitialize], [RCL_LIBS="$RCL_LIBS -lXt"], [], [$RCL_LIBS])
  AC_CHECK_LIB([Xmu], [XmuAddInitializer], [RCL_LIBS="$RCL_LIBS -lXmu"], [], [$RCL_LIBS])
  AC_CHECK_LIB([Xext], [XmbufCreateBuffers], [RCL_LIBS="$RCL_LIBS -lXext"], [], [$RCL_LIBS])
  AC_CHECK_LIB([Xpm], [XpmReadFileToPixmap], [RCL_LIBS="$RCL_LIBS -lXpm"], [], [$RCL_LIBS])
  AC_CHECK_LIB([Xaw], [XawInitializeWidgetSet], [RCL_LIBS="$RCL_LIBS -lXaw"], [], [$RCL_LIBS])
  CPPFLAGS="$save_CPPFLAGS"
  LDFLAGS="$save_LDFLAGS"
  LIBS="$save_LIBS"
fi

#AC_CHECK_LIB([posix4], [sched_yield], [RCL_LIBS="$RCL_LIBS -lposix4"])

AX_EXTRA_DIST
AX_UPLOAD([ncftpput -v upload.sourceforge.net /incoming %%s])
AX_RPM_GEN_SPEC([spec.tmpl],
            [The RoboCup Soccer Simulator LogPlayer],
            [GPL],
            [Applications/Engineering],
            [http://sserver.sf.net],
            [http://prdownloads.sourceforge.net/sserver/],
[The RoboCup Soccer Simulator LogPlaeyr (rcsslogplayer) is a logplayer program for the RCSSServer.])
AX_DIST_RPM([spec.tmpl])
#AX_DIST_MSI([Setup/Release/${PACKAGE_NAME}Setup.msi])
AX_CVS([:pserver:anonymous@cvs.sourceforge.:net:/cvsroot/sserver])


AC_CONFIG_FILES([Makefile
                 rcsslogplayer/Makefile
                 src/Makefile])
#AC_CONFIG_SUBDIRS([rcgparserexample])
AC_OUTPUT
