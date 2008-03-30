ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = rcsslogplayer src .

EXTRA_DIST = \
BUGS \
rcgparserexample/AUTHORS \
rcgparserexample/BUGS \
rcgparserexample/COPYING \
rcgparserexample/ChangeLog \
rcgparserexample/INSTALL \
rcgparserexample/Makefile.am \
rcgparserexample/NEWS \
rcgparserexample/README \
rcgparserexample/acinclude.m4 \
rcgparserexample/aclocal.m4 \
rcgparserexample/bootstrap \
rcgparserexample/configure.ac \
rcgparserexample/src/Makefile.am \
rcgparserexample/src/rcgparserexample.cpp

CLEANFILES = \
*~ \
core \
*.tar.gz \
libtool.tmp

#DISTCLEANFILES = @INCLUDE_MK_FILE@

#@INCLUDE_MK@