
bin_PROGRAMS = \
	rcg2xml \
	rcgsplit \
	rcg3to4 \
	rcg4to3

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


rcg3to4_SOURCES = rcg3to4.cpp

rcg3to4_CPPFLAGS =
rcg3to4_CXXFLAGS =
rcg3to4_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcg3to4_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


rcg4to3_SOURCES = rcg4to3.cpp

rcg4to3_CPPFLAGS =
rcg4to3_CXXFLAGS =
rcg4to3_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcg4to3_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


AM_CPPFLAGS = -I$(top_srcdir)
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDFLAGS =


pkgdata_DATA = \
	rcg.xsd \
	rcg-0.1.xsd


EXTRA_DIST =

CLEANFILES = *~
