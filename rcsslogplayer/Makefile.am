
lib_LTLIBRARIES = librcssrcgparser.la

librcssrcgparser_la_SOURCES = \
	gzfstream.cpp \
	parser.cpp \
	types.cpp \
	util.cpp

librcssrcgparserincludedir = $(includedir)/rcsslogplayer

librcssrcgparserinclude_HEADERS = \
	gzfstream.h \
	parser.h \
	handler.h \
	util.h \
	types.h

librcssrcgparser_la_LDFLAGS = -version-info 3:0:0

pkgdata_DATA =

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -W -Wall
AM_CXXFLAGS = -W -Wall
AM_LDFLAGS =

EXTRA_DIST = \
	rcsslogplayer.pro

CLEANFILES = *~
