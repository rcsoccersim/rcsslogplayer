# SYNOPSIS
#
#   AX_XAW7
#
# DESCRIPTION
#
#   The following shell variable is set to either "yes" or "no":
#
#     have_xaw7
#
#   Additionally, the following variables are exported:
#
#     XAW7_CFLAGS
#     XAW7_CXXFLAGS
#     XAW7_LDFLAGS
#     XAW7_LDADD
#     XAW7_MOC

AC_DEFUN([AX_XAW7],
[
  AC_REQUIRE([AC_PROG_CXX])

  AC_MSG_CHECKING(for Xaw7)

  XAW7_REQUIRED_VERSION=ifelse([$1], ,1.0.0,$1)
  have_xaw7="no"

  AC_PATH_PROG(PKG_CONFIG, pkg-config, no)
  if test "x$PKG_CONFIG" = "xno"; then
    AC_MSG_WARN(You have to install pkg-config to compile $PACKAGENAME-$VERSION.)
    exit 1
  fi

  PKG_CHECK_MODULES(Xaw7,
                    xaw7 >= $XAW7_REQUIRED_VERSION,
                    have_xaw7="yes",
                    have_xaw7="no")

  if test "$have_xaw7" = "no"; then
    AC_MSG_WARN([The Xaw7 library >= [$XAW7_REQUIRED_VERSION] could not be found.])
    exit 1
  fi

  XAW7_CFLAGS=$($PKG_CONFIG --cflags xaw7)
  XAW7_CXXFLAGS="$XAW7_CFLAGS"
  XAW7_CPPFLAGS=""
  XAW7_LDFLAGS=$($PKG_CONFIG --libs-only-L xaw7)
  XAW7_LDADD="$($PKG_CONFIG --libs-only-other xaw7)$($PKG_CONFIG --libs-only-l xaw7)"
  AC_MSG_NOTICE([set XAW7_CXXFLAGS... $XAW7_CXXFLAGS])
  AC_MSG_NOTICE([set XAW7_LDFLAGS... $XAW7_LDFLAGS])
  AC_MSG_NOTICE([set XAW7_LDADD... $XAW7_LDADD])
  AC_SUBST(XAW7_CFLAGS)
  AC_SUBST(XAW7_CXXFLAGS)
  AC_SUBST(XAW7_CPPFLAGS)
  AC_SUBST(XAW7_LDFLAGS)
  AC_SUBST(XAW7_LDADD)
])
