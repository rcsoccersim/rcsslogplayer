// -*-c++-*-

/***************************************************************************
                                 rcgparser.cpp
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "rcgparser.hpp"
#include "rcgdatahandler.hpp"

#include <iostream>
#include <string>

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_WINDOWS_H
#include <windows.h>
#endif

namespace rcss {

RCGParser::RCGParser( RCGDataHandler & handler )
    : M_handler( handler )
{

}


bool
RCGParser::doParse( std::istream & strm )
{
    if ( strm.tellg() == std::streampos( 0 )
         && ! parseBegin( strm ) )
    {
        return false;
    }

    if ( M_handler.getLogVersion() >= REC_VERSION_4 )
    {
        return parseLines( strm );
    }

    return parseNext( strm );
}

bool
RCGParser::parseBegin( std::istream & strm )
{
    char buf[4];
    strm.read( buf, 4 );
    if ( strm.good() )
    {
        if( buf[ 0 ] == 'U'
            && buf[ 1 ] == 'L'
            && buf[ 2 ] == 'G' )
        {
            int ver = static_cast< int >( buf[3] );
            if ( ver != 2 && ver != 3 )
            {
                ver -= static_cast< int >( '0' );
            }
            std::cerr << "RCGParser::parseBegin version = " << ver
                      << std::endl;

            M_handler.handleLogVersion( ver );
        }
        else
        {
            std::cerr << "RCGParser::parseBegin version = " << 1
                      << std::endl;

            strm.seekg( 0 );
            M_handler.handleLogVersion( 1 );
        }
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseNext( std::istream & strm )
{
    if ( M_handler.getLogVersion() == REC_OLD_VERSION )
    {
        return parseDispInfo( strm );
    }
    else
    {
        return parseMode( strm );
    }
}

bool
RCGParser::parseDispInfo( std::istream & strm )
{
    std::streampos pos = strm.tellg();
    dispinfo_t info;
    strm.read( reinterpret_cast< char * >( &info ), sizeof( dispinfo_t ) );
    if ( strm.good() )
    {
        M_handler.handleDispInfo( pos, info );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseMode( std::istream & strm )
{
    std::streampos pos = strm.tellg();
    short mode;
    strm.read( reinterpret_cast< char * >( &mode ), sizeof( mode ) );
    if ( strm.good() )
    {
        return parseItem( strm, ntohs( mode ), pos );
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseItem( std::istream & strm,
                      short mode,
                      std::streampos pos )
{
    switch( mode ) {
    case NO_INFO:
        return true;
    case SHOW_MODE:
        return parseShowInfo( strm, pos );
    case MSG_MODE:
        return parseMsgInfo( strm, pos );
    case PM_MODE:
        return parsePlayMode( strm, pos );
    case TEAM_MODE:
        return parseTeamInfo( strm, pos );
    case PARAM_MODE:
        return parseServerParams( strm, pos );
    case PPARAM_MODE:
        return parsePlayerParams( strm, pos );
    case PT_MODE:
        return parsePlayerType( strm, pos );
    default:
        throw std::string( "Unknown mode\n" );
    }
}

bool
RCGParser::parseShowInfo( std::istream & strm,
                          std::streampos pos )
{
    if ( M_handler.getLogVersion() == REC_VERSION_3 )
    {
        short_showinfo_t2 info;
        strm.read( reinterpret_cast< char * >( &info ), sizeof( short_showinfo_t2 ) );
        if ( strm.good() )
        {
            M_handler.handleShowInfo( pos, info );
            return true;
        }
        else
        {
            return strmErr( strm );
        }
    }
    else
    {
        showinfo_t info;
        strm.read( reinterpret_cast< char * >( &info ), sizeof( showinfo_t ) );
        if ( strm.good() )
        {
            M_handler.handleShowInfo( pos, info );
            return true;
        }
        else
        {
            return strmErr( strm );
        }
    }
}

bool
RCGParser::parseMsgInfo( std::istream & strm,
                         std::streampos pos )
{
    short board;
    strm.read( reinterpret_cast< char * >( &board ), sizeof( short ) );
    if( strm.good() )
    {
        short len;
        strm.read( reinterpret_cast< char * >(  &len ), sizeof( short ) );
        if( strm.good() )
        {
            len = ntohs( len );
            char* msg = new char[ len ];
            strm.read( msg, len );
            if( strm.good() )
            {
                if( msg[ len - 1 ] == 0 )
                    len = strlen( msg );
                // copy the message to a string and delete the
                // original.  That was if handleMsgInfo throws,
                // the memory won't be lost.
                std::string msgstr( msg );
                delete[] msg;
                M_handler.handleMsgInfo( pos, board, msgstr );
                return true;
            }
            else
                delete[] msg;
        }
    }
    return strmErr( strm );
}

bool
RCGParser::parsePlayMode( std::istream & strm,
                          std::streampos pos )
{
    char playmode;
    strm.read( &playmode, sizeof( char ) );
    if ( strm.good() )
    {
        M_handler.handlePlayMode( pos, playmode );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseTeamInfo( std::istream & strm,
                          std::streampos pos )
{
    team_t team_info[ 2 ];
    strm.read( reinterpret_cast< char * >( team_info ), sizeof( team_t ) * 2 );
    if ( strm.good() )
    {
        M_handler.handleTeamInfo( pos, team_info[ 0 ], team_info[ 1 ] );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parseServerParams( std::istream & strm,
                              std::streampos pos )
{
    server_params_t params;
    strm.read( reinterpret_cast< char * >( &params ), sizeof( params ) );
    if ( strm.good() )
    {
        M_handler.handleServerParams( pos, params );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parsePlayerParams( std::istream & strm,
                              std::streampos pos )
{
    player_params_t params;
    strm.read( reinterpret_cast< char * >( &params ), sizeof( params ) );
    if( strm.good() )
    {
        M_handler.handlePlayerParams( pos, params );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::parsePlayerType( std::istream & strm,
                            std::streampos pos )
{
    player_type_t type;
    strm.read( reinterpret_cast< char * >( &type ), sizeof( type ) );
    if ( strm.good() )
    {
        M_handler.handlePlayerType( pos, type );
        return true;
    }
    else
    {
        return strmErr( strm );
    }
}

bool
RCGParser::strmErr( std::istream & strm )
{
    if ( strm.eof() )
    {
        M_handler.handleEOF();
    }
    return false;
}


bool
RCGParser::parseLines( std::istream & strm )
{
    int n_line = 0;

    std::string line_buf;
    line_buf.reserve( 8192 );

    while ( std::getline( strm, line_buf ) )
    {
        ++n_line;
        if ( line_buf.empty() ) continue;

        if ( line_buf.compare( 0, 6, "(show " ) == 0 )
        {
            M_handler.handleShowLine( line_buf );
        }
        else if ( line_buf.compare( 0, 5, "(msg " ) == 0 )
        {
            M_handler.handleMsgLine( line_buf );
        }
        else if ( line_buf.compare( 0, 10, "(playmode " ) == 0 )
        {
            M_handler.handlePlayModeLine( line_buf );
        }
        else if ( line_buf.compare( 0, 6, "(team " ) == 0 )
        {
            M_handler.handleTeamLine( line_buf );
        }
        else if ( line_buf.compare( 0, 13, "(player_type " ) == 0 )
        {
            M_handler.handlePlayerTypeLine( line_buf );
        }
        else if ( line_buf.compare( 0, 14, "(server_param " ) == 0 )
        {
            M_handler.handleServerParamLine( line_buf );
        }
        else if ( line_buf.compare( 0, 14, "(player_param " ) == 0 )
        {
            M_handler.handlePlayerParamLine( line_buf );
        }
        else
        {
            //throw std::string( "Unknown info " ) + line_buf.substr( 0, 10 ) + "\n";
            std::cerr << "RCGParser: Unknown info at line " << n_line
                      << "\"" << line_buf << "\""
                      << std::endl;
        }
    }

    M_handler.handleEOF();

    return false;

}

}
