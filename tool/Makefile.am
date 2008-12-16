
bin_PROGRAMS = \
	rcg2xml \
	rcgsplit \
	rcgconvert

#bin_SCRIPTS = \
#	rcg3to4 \
#	rcg4to3

rcg2xml_SOURCES = \
	rcg2xml.cpp

rcg2xml_CPPFLAGS = -I$(top_srcdir)
rcg2xml_CXXFLAGS = -Wall
rcg2xml_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcg2xml_LDADD = -lrcssrcgparser


rcgsplit_SOURCES = rcgsplit.cpp

rcgsplit_CPPFLAGS = -I$(top_srcdir)
rcgsplit_CXXFLAGS = -Wall
rcgsplit_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcgsplit_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


##rcg3to4_SOURCES = rcg3to4.cpp
##
##rcg3to4_CPPFLAGS = -I$(top_srcdir)
##rcg3to4_CXXFLAGS = -Wall
##rcg3to4_LDFLAGS = -L$(top_builddir)/rcsslogplayer
##rcg3to4_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


##rcg4to3_SOURCES = rcg4to3.cpp
##
##rcg4to3_CPPFLAGS = -I$(top_srcdir)
##rcg4to3_CXXFLAGS = -Wall
##rcg4to3_LDFLAGS = -L$(top_builddir)/rcsslogplayer
##rcg4to3_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


rcgconvert_SOURCES = rcgconvert.cpp

rcgconvert_CPPFLAGS = -I$(top_srcdir)
rcgconvert_CXXFLAGS = -Wall
rcgconvert_LDFLAGS = -L$(top_builddir)/rcsslogplayer
rcgconvert_LDADD = -lrcssrcgparser $(BOOST_PROGRAM_OPTIONS_LIB)


AM_CPPFLAGS =
AM_CFLAGS = -Wall
AM_CXXFLAGS = -Wall
AM_LDFLAGS =


pkgdata_DATA = \
	rcg.xsd \
	rcg-0.1.xsd


EXTRA_DIST = \
	rcg.xsd \
	rcg-0.1.xsd

CLEANFILES = *~
