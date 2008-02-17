// -*-c++-*-

/***************************************************************************
                                rcgparser.hpp
                         Class for parsing rcg files
                             -------------------
    begin                : 18-FEB-2003
    copyright            : (C) 2003 by The RoboCup Soccer Server
                           Maintenance Group.
    email                : sserver-admin@lists.sourceforge.net
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU LGPL as published by the Free Software  *
 *   Foundation; either version 2 of the License, or (at your option) any  *
 *   later version.                                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef RCGPARSER_HPP
#define RCGPARSER_HPP


#include <rcssbase/parser.h>

#include <string>
#include <map>

namespace rcss {

class RCGDataHandler;

class RCGParser
    : public Parser {
private:
    RCGDataHandler & M_handler;

    // not used
    RCGParser();
    RCGParser( const RCGParser & );
    RCGParser & operator=( const RCGParser & );

public:

    typedef std::map< std::string, std::string > ParamMap;

    explicit
    RCGParser( RCGDataHandler & handler );

    ~RCGParser()
      { }

private:
    bool doParse( std::istream & strm );

    bool parseBegin( std::istream & strm );

    bool parseDispInfo( std::istream & strm );

    bool parseMode( std::istream & strm );

    bool parseNext( std::istream & strm );

    bool parseItem( std::istream & strm,
                    short mode,
                    std::streampos pos );

    bool parseShowInfo( std::istream & strm,
                        std::streampos pos );

    bool parseMsgInfo( std::istream & strm,
                       std::streampos pos );

    bool parsePlayMode( std::istream & strm,
                        std::streampos pos );

    bool parseTeamInfo( std::istream & strm,
                        std::streampos pos );

		bool parseServerParams( std::istream & strm,
                            std::streampos pos );

    bool parsePlayerParams( std::istream & strm,
                            std::streampos pos );

    bool parsePlayerType( std::istream & strm,
                          std::streampos pos );

    bool strmErr( std::istream & strm );


    // version 4
    bool parseLines( std::istream & strm );

    bool parseShowLine( const int n_line,
                        const std::string & line );
    bool parseMsgLine( const int n_line,
                       const std::string & line );
    bool parsePlayModeLine( const int n_line,
                            const std::string & line );
    bool parseTeamLine( const int n_line,
                        const std::string & line );
    bool parseServerParamLine( const int n_line,
                               const std::string & line );
    bool parsePlayerParamLine( const int n_line,
                               const std::string & line );
    bool parsePlayerTypeLine( const int n_line,
                              const std::string & line );

    bool parseParamLine( const std::string & line,
                         std::string & param_name,
                         ParamMap & param_map );

};

}


#endif
