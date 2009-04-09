
if BUILD_CLASSIC
BUILD_PROGS = rcsslogplayer_classic
BUILD_SCRPTS = rcsslogplay_classic
endif

bin_PROGRAMS = $(BUILD_PROGS)

bin_SCRIPTS = $(BUILD_SCRPTS)

rcsslogplayer_classic_SOURCES = \
	addr.cpp \
	socket.cpp \
	udpsocket.cpp \
	netif.cpp \
	controler.cpp \
	logplayer.cpp \
	nowindow.cpp \
	main.cpp

noinst_HEADERS = \
	addr.hpp \
	socket.hpp \
	udpsocket.hpp \
	netif.h \
	controler.h \
	logplayer.h \
	fallback.h

rcsslogplayer_classic_CPPFLAGS = $(CLASSIC_CPPFLAGS) -I$(top_srcdir)
rcsslogplayer_classic_CXXFLAGS = -W -Wall $(CLASSIC_CXXFLAGS)
rcsslogplayer_classic_LDFLAGS = -L$(top_builddir)/rcsslogplayer $(RCSSBASE_LDFLAGS) $(CLASSIC_LDFLAGS)
rcsslogplayer_classic_LDADD = $(CLASSIC_LIBS) -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)

pkgdata_DATA =

AM_CPPFLAGS =
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LDFLAGS =


EXTRA_DIST = \
	rcsslogplay_classic \
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
	go.xbm

CLEANFILES = *~
