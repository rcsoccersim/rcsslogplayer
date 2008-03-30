
lib_LTLIBRARIES = librcssrcgparser.la

librcssrcgparser_la_SOURCES = \
	parser.cpp \
	util.cpp

librcssrcgparserincludedir = $(includedir)/rcg

librcssrcgparserinclude_HEADERS = \
	parser.h \
	handler.h \
	util.h \
	types.h

librcssrcgparser_la_LDFLAGS = -version-info 2:0:0

pkgdata_DATA =

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDFLAGS =

CLEANFILES = 	*~
