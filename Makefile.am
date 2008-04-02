ACLOCAL_AMFLAGS=-I m4

LIBTOOL_DEPS = @LIBTOOL_DEPS@
libtool:	$(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

SUBDIRS = rcsslogplayer tool classic .

EXTRA_DIST = \
	icons/ff.xpm\
	icons/play.xpm \
	icons/plus.xpm \
	icons/stop.xpm \
	icons/minus.xpm \
	icons/rev.xpm \
	icons/rew.xpm \
	icons/rec.xpm \
	icons/blank.xpm \
	icons/quit.xpm \
	icons/go.xpm

CLEANFILES = *~
