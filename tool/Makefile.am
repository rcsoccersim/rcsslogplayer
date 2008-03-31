
bin_PROGRAMS = \
	rcg2xml \
	rcgsplit

rcg2xml_SOURCES = \
	rcg2xml.cpp

rcg2xml_CPPFLAGS =
rcg2xml_CXXFLAGS =
rcg2xml_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcg2xml_LDADD = -lrcssrcgparser

rcgsplit_SOURCES = rcgsplit.cpp

rcgsplit_CPPFLAGS =
rcgsplit_CXXFLAGS =
rcgsplit_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcgsplit_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


pkgdata_DATA =

AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDFLAGS =


EXTRA_DIST = \
	rcg.xsd \
	rcg-0.1.xsd

CLEANFILES = *~
