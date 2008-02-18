// -*-c++-*-

/***************************************************************************
                             rcgparserexample.cpp
                              Example rcgparser
                             -------------------
    begin                : 24-MAR-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is freeware; you can redistribute it and/or modify       *
 *   however you like and without restriction.                             *
 *                                                                         *
 ***************************************************************************/


#include <rcsslogplayer/rcgparser.hpp>
#include <rcsslogplayer/rcgdatahandler.hpp>
#include <netinet/in.h>


class TimeWriter
    : public rcss::RCGDataHandler {
public:
    TimeWriter()
      { }

private:
    void
    doHandleLogVersion( int ver )
      {
          m_ver = ver;
      }

    int
    doGetLogVersion() const
      {
          return m_ver;
      }

    void
    doHandleShowInfo( std::streampos,
                      const short_showinfo_t2 & info )
      {
          std::cout << "time=\"" << ntohs( info.time ) << "\">\n";
      }

    void
    doHandleShowInfo( std::streampos,
                      const showinfo_t & info )
      {
          std::cout << "time=\"" << ntohs( info.time ) << "\">\n";
      }

    void
    doHandleDispInfo( std::streampos,
                      const dispinfo_t & info )
      {
          if ( ntohs( info.mode ) == SHOW_MODE )
          {
              std::cout << "time=\"" << ntohs( info.body.show.time ) << "\">\n";
          }
      }

    void doHandleShowBegin( const int time )
      {
          std::cout << "time=\"" << time << "\">\n";
      }

    int m_ver;
};

int main()
{
    TimeWriter writer;
    rcss::RCGParser parser( writer );
    try
    {
        while ( parser.parse( std::cin ) )
        {
            // do nothing
        }
    }
    catch( const std::string & e )
    {
        std::cerr << e << std::endl;
    }
    catch( ... )
    {
        std::cerr << "Unknown error occured\n";
    }
}
