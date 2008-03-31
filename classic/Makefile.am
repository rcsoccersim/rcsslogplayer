
bin_PROGRAMS = \
	rcsslogplayer_classic

bin_SCRIPTS = rcsslogplay

rcsslogplayer_classic_SOURCES = \
	netif.cpp \
	controler.cpp \
	logplayer.cpp \
	nowindow.cpp \
	main.cpp

noinst_HEADERS = \
	netif.h \
	controler.h \
	logplayer.h \
	fallback.h

rcsslogplayer_classic_CPPFLAGS = $(RCL_CPPFLAGS)
rcsslogplayer_classic_CXXFLAGS = $(RCL_CXXFLAGS)
rcsslogplayer_classic_LDFLAGS = $(RCL_LDFLAGS) -L$(top_builddir)/rcsslogplayer
rcsslogplayer_classic_LDADD = $(RCL_LIBS) -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)

pkgdata_DATA =

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDFLAGS =


EXTRA_DIST = \
	ff.xbm \
	play.xbm \
	plus.xbm \
	stop.xbm \
	minus.xbm \
	rev.xbm \
	rew.xbm \
	rec.xbm \
	blank.xbm \
	quit.xbm \
	go.xbm \
	rcsslogplay

CLEANFILES = *~
