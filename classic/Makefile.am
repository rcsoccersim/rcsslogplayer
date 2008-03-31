
lib_LTLIBRARIES = librcssrcgparser.la

bin_PROGRAMS = \
	rcsslogplayer

bin_SCRIPTS = rcsslogplay


librcssrcgparser_la_SOURCES = \
	rcgparser.cpp

librcssrcgparserincludedir = $(includedir)/rcsslogplayer

librcssrcgparserinclude_HEADERS = \
	rcgparser.hpp \
	rcgdatahandler.hpp \
	types.h

librcssrcgparser_la_LDFLAGS = -version-info 1:0:1

rcsslogplayer_SOURCES = \
	netif.cpp \
	controler.cpp \
	logplayer.cpp \
	nowindow.cpp \
	main.cpp

noinst_HEADERS = \
	netif.h \
	controler.h \
	logplayer.h \
	fallback.h \
	utility.h

rcsslogplayer_CPPFLAGS = $(RCL_CPPFLAGS)
rcsslogplayer_CXXFLAGS = $(RCL_CXXFLAGS)
rcsslogplayer_LDFLAGS = $(RCL_LDFLAGS)
rcsslogplayer_LDADD = $(RCL_LIBS) librcssrcgparser.la $(BOOST_PROGRAM_OPTIONS_LIB)

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
	rcg.xsd \
	rcg-0.1.xsd \
	rcsslogplay \
	ac_lib_rcgparser.m4

CLEANFILES = *~
